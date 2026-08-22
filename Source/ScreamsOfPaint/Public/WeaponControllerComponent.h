// WeaponControllerComponent.h
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponBase.h"
#include "RangeWeaponBase.h"
#include "WeaponControllerComponent.generated.h"

class URecoilComponent;

UCLASS(ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class SCREAMSOFPAINT_API UWeaponControllerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UWeaponControllerComponent();

	UFUNCTION(BlueprintCallable) void Attack();
	UFUNCTION(BlueprintCallable) void Release();
	UFUNCTION(BlueprintCallable) bool AllowRapidAttack() const;
	UFUNCTION(BlueprintCallable) void SwapWeapon(float ScrollValue);
	UFUNCTION(BlueprintCallable) void ReloadWeaponData();   // bind F5 cho GD
	UFUNCTION(BlueprintPure)     AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	virtual void BeginPlay() override;

private:
	void InitializeWeaponList();
	void ApplyDataToWeapons();
	void SelectWeapon(int32 Index);

	UPROPERTY() TArray<AWeaponBase*>      EquippedWeapons;
	UPROPERTY() TArray<ARangeWeaponBase*> RangeWeapons;
	UPROPERTY() AWeaponBase*              CurrentWeapon = nullptr;
	UPROPERTY() URecoilComponent*         Recoil = nullptr;
	int32 CurrentIndex = 0;
};