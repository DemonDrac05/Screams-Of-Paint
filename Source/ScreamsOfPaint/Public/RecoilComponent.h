#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDataTypes.h"
#include "RecoilComponent.generated.h"

class UCameraComponent;
class APlayerController;

UCLASS(ClassGroup = (Weapon), meta = (BlueprintSpawnableComponent))
class SCREAMSOFPAINT_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URecoilComponent();

	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void Setup(USceneComponent* InRecoilPivot, USceneComponent* InWeaponRoot);

	void RefreshData(const FRecoilData& InData);

	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ApplyRecoil();

	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ApplyRecoilScaled(float Scale);

	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void SetRamp(float InPerShot, float InMax);

	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ResetRamp();

	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ClearRecoil();

	FORCEINLINE float GetRampAlpha() const { return Ramp; }

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AimPunchRatio = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AimRecoverRatio = 0.85f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "1.0"))
	float AimKickSpeedMul = 7.f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "0.0"))
	float ViewPunchScale = 1.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "1.0"))
	float ViewSpringMul = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "0.1", ClampMax = "1.5"))
	float ViewDampRatio = 0.48f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "1.0"))
	float WeaponSpringMul = 1.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "0.1", ClampMax = "1.5"))
	float WeaponDampRatio = 0.42f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Feel", meta = (ClampMin = "0.0"))
	float FovPunchPerVertical = 0.18f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|HitStop")
	bool bUseHitStop = true;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|HitStop")
	float HitStopMinVertical = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|HitStop", meta = (ClampMin = "0.0", ClampMax = "0.15"))
	float HitStopMaxDuration = 0.045f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|HitStop", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float HitStopDilation = 0.08f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|HitStop", meta = (ClampMin = "0.0"))
	float HitStopMinInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil|Setup")
	bool bPivotAppliesLookPitch = false;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void StepAim(float Dt);
	void StepView(float Dt);
	void StepWeapon(float Dt);
	void StepFov(float Dt);
	void ApplyTransforms();

	void AddControlRotation(float PitchDeg, float YawDeg);
	APlayerController* GetPC() const;

	void BeginHitStop(float RealDuration);
	void EndHitStop();

	UPROPERTY() USceneComponent*  RecoilPivot = nullptr;
	UPROPERTY() USceneComponent*  WeaponRoot  = nullptr;
	UPROPERTY() UCameraComponent* Camera      = nullptr;

	FRecoilData Data;

	// ---------- AIM PUNCH ----------
	FVector2D AimTarget  = FVector2D::ZeroVector;
	FVector2D AimCurrent = FVector2D::ZeroVector;

	// ---------- VIEW PUNCH ----------
	FRotator ViewPunch = FRotator::ZeroRotator;
	FRotator ViewVel   = FRotator::ZeroRotator;

	// ---------- WEAPON PUNCH ----------
	FVector  WeaponLoc    = FVector::ZeroVector;
	FVector  WeaponLocVel = FVector::ZeroVector;
	FRotator WeaponRot    = FRotator::ZeroRotator;
	FRotator WeaponRotVel = FRotator::ZeroRotator;
	FVector  WeaponOrgLoc = FVector::ZeroVector;
	FRotator WeaponOrgRot = FRotator::ZeroRotator;

	// ---------- FOV ----------
	float FovPunch   = 0.f;
	float FovVel     = 0.f;
	float AppliedFov = 0.f;

	// ---------- RAMP ----------
	float Ramp         = 0.f;
	float RampPerShot  = 0.f;
	float RampMax      = 0.f;
	float LastShotTime = -1000.f;

	int32 YawSign = 1;

	float LastHitStopRealTime = -1000.f;
	FTimerHandle HitStopHandle;
};