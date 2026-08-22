#include "RecoilComponent.h"
#include "GameFramework/Pawn.h"

URecoilComponent::URecoilComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void URecoilComponent::Setup(USceneComponent* InRecoilPivot, USceneComponent* InWeaponRoot)
{
    RecoilPivot = InRecoilPivot;
    WeaponRoot  = InWeaponRoot;
    if (WeaponRoot)
    {
        WeaponOrgLoc = WeaponRoot->GetRelativeLocation();
        WeaponOrgRot = WeaponRoot->GetRelativeRotation();
    }
}

void URecoilComponent::RefreshData(const FRecoilData& InData)
{
    Data = InData;
}

void URecoilComponent::ApplyRecoil()
{
    TargetRecoil.Pitch += Data.Vertical;
    TargetRecoil.Yaw   += FMath::RandRange(-Data.Horizontal, Data.Horizontal);

    if (WeaponRoot)
    {
        WeaponRoot->AddLocalOffset(FVector(-Data.Kickback, 0.f, 0.f));
        WeaponRoot->AddLocalRotation(FRotator(Data.Rot, 0.f, 0.f));
    }
}

void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!RecoilPivot || !WeaponRoot) return;

    WeaponRoot->SetRelativeLocation(FMath::VInterpTo(
        WeaponRoot->GetRelativeLocation(), WeaponOrgLoc, DeltaTime, Data.WeaponReturnSpeed));
    WeaponRoot->SetRelativeRotation(FMath::RInterpTo(
        WeaponRoot->GetRelativeRotation(), WeaponOrgRot, DeltaTime, Data.WeaponReturnSpeed));

    TargetRecoil  = FMath::RInterpTo(TargetRecoil,  FRotator::ZeroRotator, DeltaTime, Data.CamReturnSpeed);
    CurrentRecoil = FMath::RInterpTo(CurrentRecoil, TargetRecoil,          DeltaTime, Data.CamReturnSpeed * 0.5f);

    float LookPitch = 0.f;
    if (const APawn* Pawn = Cast<APawn>(GetOwner()))
        LookPitch = FRotator::NormalizeAxis(Pawn->GetControlRotation().Pitch);

    RecoilPivot->SetRelativeRotation(
        FRotator(LookPitch + CurrentRecoil.Pitch, CurrentRecoil.Yaw, 0.f));
}