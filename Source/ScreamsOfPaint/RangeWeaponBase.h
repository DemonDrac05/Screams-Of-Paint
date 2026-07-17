#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponDataTypes.h"
#include "GameFramework/Actor.h"
#include "RangeWeaponBase.generated.h"

UCLASS(Abstract)
class SCREAMSOFPAINT_API ARangeWeaponBase : public AWeaponBase
{
	GENERATED_BODY()
public:
	ARangeWeaponBase();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Attack() override;
	virtual void Release() override;
	virtual bool AllowRapidFire() const override;
	
	bool CanFire();
	bool HasEnoughMagazine();
	
	void ResetManagazine();
	void CalculateBulletPath();
	
	FVector FindTargetPoint();
	FRangeWeaponData WeaponData;
private:
	float NextTimeToFire = 0.f;
	FTransform CameraPoint;
};
