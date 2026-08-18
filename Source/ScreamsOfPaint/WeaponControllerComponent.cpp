#include "WeaponControllerComponent.h"
#include "WeaponDataLibrary.h"
#include "RecoilComponent.h"

UWeaponControllerComponent::UWeaponControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponControllerComponent::BeginPlay()
{
    Super::BeginPlay();
    Recoil = GetOwner()->FindComponentByClass<URecoilComponent>();
    InitializeWeaponList();
    ApplyDataToWeapons();
    SelectWeapon(0);
}

void UWeaponControllerComponent::InitializeWeaponList()
{
    TArray<UChildActorComponent*> ChildActorComps;
    GetOwner()->GetComponents<UChildActorComponent>(ChildActorComps);

    for (UChildActorComponent* CAC : ChildActorComps)
    {
        AWeaponBase* Weapon = Cast<AWeaponBase>(CAC->GetChildActor());
        if (!Weapon) continue;

        EquippedWeapons.Add(Weapon);
        if (ARangeWeaponBase* RangeWeapon = Cast<ARangeWeaponBase>(Weapon))
            RangeWeapons.Add(RangeWeapon);

        Weapon->SetActorHiddenInGame(true);
        Weapon->SetActorEnableCollision(false);
    }
}

void UWeaponControllerComponent::ApplyDataToWeapons()
{
    TArray<FRangeWeaponData> AllData;
    FString Error;
    if (!UWeaponDataLibrary::LoadRangeWeaponData(AllData, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Weapon data: %s"), *Error);
        return;
    }

    for (ARangeWeaponBase* Weapon : RangeWeapons)
    {
        const FRangeWeaponData* Found = AllData.FindByPredicate(
            [Weapon](const FRangeWeaponData& D) { return FName(*D.Id) == Weapon->GetID(); });

        if (Found) Weapon->SetData(*Found);
        else UE_LOG(LogTemp, Warning, TEXT("No data for weapon %s"), *Weapon->GetID().ToString());
    }
}

void UWeaponControllerComponent::SelectWeapon(int32 Index)
{
    if (!EquippedWeapons.IsValidIndex(Index)) return;

    if (CurrentWeapon) CurrentWeapon->SetActorHiddenInGame(true);

    CurrentIndex  = Index;
    CurrentWeapon = EquippedWeapons[Index];
    CurrentWeapon->SetActorHiddenInGame(false);

    if (const ARangeWeaponBase* RW = Cast<ARangeWeaponBase>(CurrentWeapon); RW && Recoil)
        Recoil->RefreshData(RW->GetRecoilData());
}

void UWeaponControllerComponent::SwapWeapon(float ScrollValue)
{
    if (EquippedWeapons.Num() < 2 || FMath::IsNearlyZero(ScrollValue)) return;

    const int32 Next = ScrollValue > 0.f
        ? (CurrentIndex + 1) % EquippedWeapons.Num()
        : (CurrentIndex - 1 + EquippedWeapons.Num()) % EquippedWeapons.Num();
    SelectWeapon(Next);
}

void UWeaponControllerComponent::ReloadWeaponData()
{
    ApplyDataToWeapons();
    if (const ARangeWeaponBase* RW = Cast<ARangeWeaponBase>(CurrentWeapon); RW && Recoil)
        Recoil->RefreshData(RW->GetRecoilData());
}

bool UWeaponControllerComponent::AllowRapidAttack() const
{
    return CurrentWeapon && CurrentWeapon->AllowRapidFire();
}

void UWeaponControllerComponent::Attack()  { if (CurrentWeapon) CurrentWeapon->Attack(); }
void UWeaponControllerComponent::Release() { if (CurrentWeapon) CurrentWeapon->Release(); }