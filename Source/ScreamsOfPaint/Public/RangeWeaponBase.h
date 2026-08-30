#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponDataTypes.h"
#include "GameFramework/Actor.h"
#include "RangeWeaponBase.generated.h"

class URecoilComponent;

UCLASS(Abstract)
class SCREAMSOFPAINT_API ARangeWeaponBase : public AWeaponBase
{
	GENERATED_BODY()
public:
	ARangeWeaponBase();
	virtual void SetData(const FRangeWeaponData& InData);
	
	virtual bool TryAttack() override;
	virtual void Attack_Implementation() override;
	virtual void Release_Implementation() override;
	
	FORCEINLINE const FRecoilData& GetRecoilData() const { return WeaponData.Recoil; }
protected:
	// ===== Override Functions =====
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ProcessFire();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetRecoilScale();
	
	// ===== Functions ======
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool CanFire();
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool HasEnoughMagazine();
	 
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void CalculateBulletPath();
	
	FVector FindTargetPoint() const;
	
	// ===== Data ======
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FRangeWeaponData WeaponData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	URecoilComponent* RecoilComp = nullptr;
	
	// ===== Fragment Gun ======
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Range|Fragment")
	bool bHasFinalShot = false;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Weapon|Range|Fragment")
	void PrepareFinalShot();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Weapon|Range|Fragment")
	void FireFinalShot();
	
	// ===== Magazine ======
	FORCEINLINE bool UsesMagazine() const { return MaxShotsPerMagazine > 0; }
	
	UPROPERTY(BlueprintReadOnly, Category="Weapon|Magazine")
	int32 CurrentShot = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Magazine")
	int32 MaxShotsPerMagazine = 0;
	
	UFUNCTION(BlueprintCallable, Category="Weapon|Magazine")
	void ReloadMagazine();
	
private:
	double LastFireTime = -1000.0;
	float NextTimeToFire = 0.f;
	float ReloadTimer = 0.f;
	bool bIsReloading = false;
	
	UPROPERTY()
	FTimerHandle ReloadTimerHandle;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Muzzle;
	
	UPROPERTY()
	AActor* OwningCharacter = nullptr;
};
