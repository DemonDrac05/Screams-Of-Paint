#include "PaintCoverageComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ComboTableRow.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SingleTableRow.h"
#include "SOPPaintComboSubsystem.h"

void UPaintCoverageComponent::BeginPlay()
{
    Super::BeginPlay();

    if (auto* Mesh = GetOwner()->FindComponentByClass<UMeshComponent>())
    {
        for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
        {
            UMaterialInterface* OriginalMat = Mesh->GetMaterial(i);
            if (!OriginalMat) continue;
            auto* MID = UMaterialInstanceDynamic::Create(OriginalMat, Mesh);
            Mesh->SetMaterial(i, MID);
            MeshMIDs.Add(MID);
        }
    }
    PushBlobsToMaterials();
}

void UPaintCoverageComponent::TickComponent(float DT, ELevelTick Tick,
                                            FActorComponentTickFunction* Func)
{
    Super::TickComponent(DT, Tick, Func);

    for (auto& B : Blobs)
        B.Radius = FMath::FInterpTo(B.Radius, B.TargetRadius, DT, SpreadInterpSpeed);

    PushBlobsToMaterials();
}

void UPaintCoverageComponent::PushBlobsToMaterials()
{
    auto* SkMesh = GetOwner()
        ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;

    TArray<const FPaintBlob*> Sorted;
    Sorted.Reserve(Blobs.Num());
    for (const auto& B : Blobs) Sorted.Add(&B);
    
    if (Sorted.Num() > MaxRenderBlobs)
    {
        Sorted.Sort([](const FPaintBlob& A, const FPaintBlob& B)
            { return A.Sequence > B.Sequence; });   
        Sorted.SetNum(MaxRenderBlobs);
    }
    
    Sorted.Sort([](const FPaintBlob& A, const FPaintBlob& B){ return A.Sequence < B.Sequence; });

    for (int32 i = 0; i < MaxRenderBlobs; i++)
    {
        FLinearColor PosR(0, 0, 0, 0);
        FLinearColor Col = FLinearColor::Black;

        if (i < Sorted.Num())
        {
            const FPaintBlob& B = *Sorted[i];
            FVector WorldPos = B.BoneLocalPos;
            if (SkMesh && B.BoneName != NAME_None)
            {
                const int32 BoneIdx = SkMesh->GetBoneIndex(B.BoneName);
                if (BoneIdx != INDEX_NONE)
                    WorldPos = SkMesh->GetBoneTransform(BoneIdx).TransformPosition(B.BoneLocalPos);
            }
            const float R = FMath::Clamp(B.Radius * FullBodyRadius, 4.f, FullBodyRadius); 
            
            if (bDebugLogHits && i == 0)
                UE_LOG(LogTemp, Warning, TEXT("[Paint] Blob0 R=%.1fcm  Amount=%.2f"), R, B.PaintAmount);
            
            PosR = FLinearColor(WorldPos.X, WorldPos.Y, WorldPos.Z, R);
            Col  = GetPaintLinearColor(B.Color);
        }

        const FName PosName (*FString::Printf(TEXT("PaintBlob%d"),  i));
        const FName ColName (*FString::Printf(TEXT("PaintColor%d"), i));
        for (auto* MID : MeshMIDs)
        {
            if (!MID) continue;
            MID->SetVectorParameterValue(PosName, PosR);
            MID->SetVectorParameterValue(ColName, Col);
        }
    }
}

void UPaintCoverageComponent::RegisterHit(
    FVector WorldHitPos, FName BoneName,
    EPaintColor Color, float SplatSize)
{
    if (!GetOwner()) return;

    auto* SkMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    FVector BoneLocalPos = WorldHitPos;
    if (SkMesh)
    {
        if (BoneName == NAME_None)
            BoneName = SkMesh->FindClosestBone(WorldHitPos);
        if (const int32 BoneIdx = SkMesh->GetBoneIndex(BoneName); BoneIdx != INDEX_NONE)
            BoneLocalPos = SkMesh->GetBoneTransform(BoneIdx).InverseTransformPosition(WorldHitPos);
    }
    
    if (bDebugLogHits)
        UE_LOG(LogTemp, Warning, TEXT("[Paint] Bone=%s LocalPos=(%.1f, %.1f, %.1f) Blobs=%d"),
            *BoneName.ToString(),
            BoneLocalPos.X, BoneLocalPos.Y, BoneLocalPos.Z, Blobs.Num());

    // Overcharge ──────────────────────────────────────────────
    if (bSingleTriggered.FindRef(Color))
    {
        float& OC = Overcharge.FindOrAdd(Color);
        OC = FMath::Clamp(OC + SplatSize, 0.f, OverchargeThreshold);
        if (OC >= OverchargeThreshold)
        {
            TriggerOvercharge(Color);
            bSingleTriggered[Color] = false;
        }
        return;
    }

    // Blob merge ────────────────────────────────────────────────
    auto ToWorld = [SkMesh](const FPaintBlob& B) -> FVector
    {
        if (!SkMesh || B.BoneName == NAME_None) return B.BoneLocalPos;
        const int32 BoneIdx = SkMesh->GetBoneIndex(B.BoneName);
        if (BoneIdx == INDEX_NONE) return B.BoneLocalPos;
        return SkMesh->GetBoneTransform(BoneIdx).TransformPosition(B.BoneLocalPos);
    };
    
    FPaintBlob* Same = Blobs.FindByPredicate([&](const FPaintBlob& B) {
        if (B.Color != Color) return false;
        const float Reach = FMath::Max(MergeDistance, B.TargetRadius * FullBodyRadius * 0.6f);
        return FVector::Dist(ToWorld(B), WorldHitPos) < Reach;
    });

    if (Same)
    {
        Same->PaintAmount = FMath::Min(Same->PaintAmount + SplatSize, 1.f);
        Same->TargetRadius = FMath::Pow(Same->PaintAmount, SpreadExponent);
        Same->Radius = FMath::Max(Same->Radius, Same->TargetRadius * InstantSplatFraction);
        Same->Sequence = NextSequence++;
    }
    else
    {
        FPaintBlob NewB{ BoneName, BoneLocalPos, 0.f, 0.f, 0.f, Color };
        NewB.PaintAmount    = SplatSize;
        NewB.TargetRadius   = FMath::Pow(SplatSize, SpreadExponent);
        NewB.Radius         = NewB.TargetRadius * InstantSplatFraction;
        NewB.Sequence       = NextSequence++;
        Blobs.Add(NewB);
    }

    // Coverage ─────────────────────────────────────────────────────────────
    float& Cov = Coverage.FindOrAdd(Color);
    Cov = FMath::Clamp(Cov + SplatSize, 0.f, 1.f);

    if (bEnableComboAndSingle)
    {
        // Combo check ─────────────────────────────────────────────
        const FPaintBlob& NewBlob = Same ? *Same : Blobs.Last();
        auto* Sub = GetWorld()->GetSubsystem<USOPaintComboSubsystem>();

        for (const FPaintBlob& Other : Blobs)
        {
            if (Other.Color == Color) continue;
        
            if (const float ReachCm = (NewBlob.Radius + Other.Radius) * FullBodyRadius;
                FVector::Dist(Other.BoneLocalPos, NewBlob.BoneLocalPos) > ReachCm) continue;

            FComboTableRow ComboRow;
            if (!Sub || !Sub->TryGetCombo({ Color, Other.Color }, ComboRow)) continue;

            EPaintColor OtherColor = Other.Color;
            float Intensity = FMath::Clamp(
                (Coverage.FindRef(Color) + Coverage.FindRef(OtherColor)) * 0.5f,
                0.05f, 1.f);

            TriggerCombo(ComboRow, Intensity);
            Coverage.Remove(Color);
            Coverage.Remove(OtherColor);
            Blobs.RemoveAll([Color, OtherColor](const FPaintBlob& B) {
                return B.Color == Color || B.Color == OtherColor;
            });
            return;
        }
    }
    
    // Single check ─────────────────────────────────────────────────────────
    if (Cov >= 1.f) TriggerSingle(Color);
    
    PushBlobsToMaterials();
}

void UPaintCoverageComponent::TriggerSingle(EPaintColor Color)
{
    auto* Sub = GetWorld()->GetSubsystem<USOPaintComboSubsystem>();
    auto* ASC = Cast<IAbilitySystemInterface>(GetOwner())
                    ? Cast<IAbilitySystemInterface>(GetOwner())->GetAbilitySystemComponent()
                    : nullptr;
    FSingleTableRow Row;
    if (!Sub || !ASC || !Sub->TryGetSingle(Color, Row)) return;

    ASC->ApplyGameplayEffectToSelf(Row.Effect.GetDefaultObject(), 1.f, ASC->MakeEffectContext());

    if (Row.bHasOvercharge) bSingleTriggered.Add(Color, true);
    else ClearPaintOfColor(Color);
}

void UPaintCoverageComponent::TriggerOvercharge(EPaintColor Color)
{
    auto* Sub = GetWorld()->GetSubsystem<USOPaintComboSubsystem>();
    auto* ASC = Cast<IAbilitySystemInterface>(GetOwner())
                    ? Cast<IAbilitySystemInterface>(GetOwner())->GetAbilitySystemComponent()
                    : nullptr;
    FSingleTableRow Row;
    if (ASC && Sub && Sub->TryGetSingle(Color, Row) && Row.OverchargeEffect)
        ASC->ApplyGameplayEffectToSelf(Row.OverchargeEffect.GetDefaultObject(), 1.f,
                                       ASC->MakeEffectContext());
    ClearPaintOfColor(Color);
}

void UPaintCoverageComponent::TriggerCombo(const FComboTableRow& Row, float Intensity)
{
    auto* ASC = Cast<IAbilitySystemInterface>(GetOwner())
                    ? Cast<IAbilitySystemInterface>(GetOwner())->GetAbilitySystemComponent()
                    : nullptr;
    if (ASC && Row.PrimaryEffect)
        ASC->ApplyGameplayEffectToSelf(Row.PrimaryEffect.GetDefaultObject(), Intensity,
                                       ASC->MakeEffectContext());
}

void UPaintCoverageComponent::ClearPaintOfColor(EPaintColor Color)
{
    Coverage.Remove(Color);
    Overcharge.Remove(Color);
    bSingleTriggered.Remove(Color);
    Blobs.RemoveAll([Color](const FPaintBlob& B){ return B.Color == Color; });
    PushBlobsToMaterials();
}

void UPaintCoverageComponent::ClearAllPaint()
{
    Coverage.Empty();
    Blobs.Empty();
    Overcharge.Empty();
    PushBlobsToMaterials();
}

FLinearColor UPaintCoverageComponent::GetPaintLinearColor(EPaintColor Color) const
{
    switch (Color)
    {
    case EPaintColor::Red:    return FLinearColor(1.f,   0.f,  0.f,  1.f);
    case EPaintColor::Blue:   return FLinearColor(0.f,   0.3f, 1.f,  1.f);
    case EPaintColor::Yellow: return FLinearColor(1.f,   0.9f, 0.f,  1.f);
    case EPaintColor::White:  return FLinearColor(1.f,   1.f,  1.f,  1.f);
    case EPaintColor::Black:  return FLinearColor(0.05f, 0.05f,0.1f, 1.f);
    default:                  return FLinearColor::Gray;
    }
}