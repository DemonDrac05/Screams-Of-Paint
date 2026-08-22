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
	
	virtual void Attack() override;
	virtual void Release() override;
	virtual bool AllowRapidFire() const override;
	
	FORCEINLINE const FRecoilData& GetRecoilData() const { return WeaponData.Recoil; }
protected:
	// ===== Override Functions =====
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void ProcessFire();
	
	// ===== Functions ======
	UFUNCTION(BlueprintCallable)
	bool CanFire();
	
	UFUNCTION(BlueprintCallable)
	bool HasEnoughMagazine();
	 
	UFUNCTION(BlueprintCallable)
	void ResetMagazine();
	void CalculateBulletPath();
	
	FVector FindTargetPoint() const;
	
	// ===== Data ======
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FRangeWeaponData WeaponData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	URecoilComponent* RecoilComp = nullptr;
	
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
	
	UFUNCTION()
	void FinishReload();
};
