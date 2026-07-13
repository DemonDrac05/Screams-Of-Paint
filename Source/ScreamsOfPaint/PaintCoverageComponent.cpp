// PaintCoverageComponent.cpp
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

    // Tạo MID cho MỌI slot — cả 2 slot nhận CÙNG bộ param blob,
    // nên vết sơn loang tự nhiên qua ranh giới material, không cần map slot.
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
    PushBlobsToMaterials(); // zero hết param lúc đầu
}

void UPaintCoverageComponent::TickComponent(float DT, ELevelTick Tick,
                                            FActorComponentTickFunction* Func)
{
    Super::TickComponent(DT, Tick, Func);

    for (auto& B : Blobs)
        B.Radius = FMath::FInterpTo(B.Radius, B.TargetRadius, DT, 6.f);

    // Enemy đang animate nên world pos của blob đổi mỗi frame → push mỗi tick.
    PushBlobsToMaterials();
}

// ─────────────────────────────────────────────────────────────────────────────
// Blob (bone-local) → world pos → material params.
// Param names trong material: PaintBlob0..7 (xyz = world pos, w = radius cm)
//                             PaintColor0..7 (rgb = màu sơn)
// ─────────────────────────────────────────────────────────────────────────────
void UPaintCoverageComponent::PushBlobsToMaterials()
{
    auto* SkMesh = GetOwner()
        ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;

    // Nếu nhiều hơn MaxRenderBlobs: render các blob TO NHẤT (gameplay giữ đủ)
    TArray<const FPaintBlob*> Sorted;
    Sorted.Reserve(Blobs.Num());
    for (const auto& B : Blobs) Sorted.Add(&B);
    if (Sorted.Num() > MaxRenderBlobs)
        Sorted.Sort([](const FPaintBlob& A, const FPaintBlob& B)
                    { return A.Radius > B.Radius; });

    for (int32 i = 0; i < MaxRenderBlobs; i++)
    {
        FLinearColor PosR(0, 0, 0, 0);   // w=0 → material bỏ qua blob này
        FLinearColor Col = FLinearColor::Black;

        if (i < Sorted.Num())
        {
            const FPaintBlob& B = *Sorted[i];
            FVector WorldPos = B.BoneLocalPos;
            if (SkMesh && B.BoneName != NAME_None)
            {
                const int32 BoneIdx = SkMesh->GetBoneIndex(B.BoneName);
                if (BoneIdx != INDEX_NONE)
                    WorldPos = SkMesh->GetBoneTransform(BoneIdx)
                                    .TransformPosition(B.BoneLocalPos);
            }
            const float R = FMath::Min(B.Radius * BlobRadiusScale,
                                       MaxBlobWorldRadius);
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

    // Bone local space — để vết sơn BÁM THEO xương khi enemy cử động
    auto* SkMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    FVector BoneLocalPos = WorldHitPos;
    if (SkMesh && BoneName != NAME_None)
    {
        const int32 BoneIdx = SkMesh->GetBoneIndex(BoneName);
        if (BoneIdx != INDEX_NONE)
            BoneLocalPos = SkMesh->GetBoneTransform(BoneIdx)
                                .InverseTransformPosition(WorldHitPos);
    }

    if (bDebugLogHits)
        UE_LOG(LogTemp, Warning,
            TEXT("[Paint] Bone=%s LocalPos=(%.1f, %.1f, %.1f) Blobs=%d"),
            *BoneName.ToString(),
            BoneLocalPos.X, BoneLocalPos.Y, BoneLocalPos.Z, Blobs.Num());

    // ── Overcharge (giữ nguyên) ──────────────────────────────────────────────
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

    // ── Blob merge / thêm mới ────────────────────────────────────────────────
    // "Bắn dồn 1 chỗ → blob to; bắn rải → nhiều blob nhỏ" — đúng lời GS.
    FPaintBlob* Same = Blobs.FindByPredicate([&](const FPaintBlob& B) {
        return B.Color == Color
            && B.BoneName == BoneName
            && FVector::Dist(B.BoneLocalPos, BoneLocalPos) < MergeDistance;
    });

    if (Same) Same->TargetRadius += SplatSize;
    else      Blobs.Add({ BoneName, BoneLocalPos, 0.f, SplatSize, Color });

    // ── Coverage ─────────────────────────────────────────────────────────────
    float& Cov = Coverage.FindOrAdd(Color);
    Cov = FMath::Clamp(Cov + SplatSize, 0.f, 1.f);

    // ── Combo check (giữ nguyên) ─────────────────────────────────────────────
    const FPaintBlob& NewBlob = Same ? *Same : Blobs.Last();
    auto* Sub = GetWorld()->GetSubsystem<USOPaintComboSubsystem>();

    for (const FPaintBlob& Other : Blobs)
    {
        if (Other.Color == Color) continue;
        if (FVector::Dist(Other.BoneLocalPos, NewBlob.BoneLocalPos)
            > NewBlob.Radius + Other.Radius) continue;

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
        return; // PushBlobsToMaterials ở Tick sẽ tự xóa vết trên material
    }

    // ── Single check ─────────────────────────────────────────────────────────
    if (Cov >= 1.f) TriggerSingle(Color);
}

void UPaintCoverageComponent::TriggerSingle(EPaintColor Color)
{
    auto* Sub = GetWorld()->GetSubsystem<USOPaintComboSubsystem>();
    auto* ASC = Cast<IAbilitySystemInterface>(GetOwner())
                    ? Cast<IAbilitySystemInterface>(GetOwner())->GetAbilitySystemComponent()
                    : nullptr;
    FSingleTableRow Row;
    if (!Sub || !ASC || !Sub->TryGetSingle(Color, Row)) return;

    ASC->ApplyGameplayEffectToSelf(Row.Effect.GetDefaultObject(), 1.f,
                                   ASC->MakeEffectContext());

    if (Row.bHasOvercharge) bSingleTriggered.Add(Color, true);
    else                    ClearAllPaint();
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
    ClearAllPaint();
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

void UPaintCoverageComponent::ClearAllPaint()
{
    Coverage.Empty();
    Blobs.Empty();
    Overcharge.Empty();
    PushBlobsToMaterials(); // zero param → vết biến mất ngay
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