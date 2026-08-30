#include "RecoilComponent.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

namespace RecoilSpring
{
	template<typename T>
	FORCEINLINE void Step(T& Value, T& Velocity, const T& Target,
	                      float Omega, float DampRatio, float Dt)
	{
		const T Accel = (Target - Value) * (Omega * Omega) - Velocity * (2.f * DampRatio * Omega);
		Velocity += Accel * Dt;
		Value    += Velocity * Dt;
	}
}

URecoilComponent::URecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup    = TG_PostPhysics;
}

void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Camera && GetOwner())
	{
		Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
	}

	if (!RecoilPivot && Camera)
	{
		RecoilPivot = Camera->GetAttachParent();
		UE_LOG(LogTemp, Warning,
			TEXT("[Recoil] Chua goi Setup(). Tam dung parent cua Camera lam pivot: %s"),
			RecoilPivot ? *RecoilPivot->GetName() : TEXT("NULL"));
	}
}

void URecoilComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitStopHandle);
		UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
	}
	Super::EndPlay(EndPlayReason);
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

	if (!Camera && GetOwner())
	{
		Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
	}
}

void URecoilComponent::RefreshData(const FRecoilData& InData)
{
	Data = InData;

	Data.CamReturnSpeed    = FMath::Max(Data.CamReturnSpeed,    0.5f);
	Data.WeaponReturnSpeed = FMath::Max(Data.WeaponReturnSpeed, 0.5f);
}

void URecoilComponent::SetRamp(float InPerShot, float InMax)
{
	RampPerShot = FMath::Max(InPerShot, 0.f);
	RampMax     = FMath::Max(InMax,     0.f);
	Ramp        = 0.f;
}

void URecoilComponent::ResetRamp()
{
	Ramp = 0.f;
}

void URecoilComponent::ClearRecoil()
{
	AimTarget = AimCurrent = FVector2D::ZeroVector;
	ViewPunch = ViewVel = FRotator::ZeroRotator;
	WeaponLoc = WeaponLocVel = FVector::ZeroVector;
	WeaponRot = WeaponRotVel = FRotator::ZeroRotator;
	FovPunch  = FovVel = 0.f;
	Ramp      = 0.f;
}

// ============================================================================
// FIRE
// ============================================================================
void URecoilComponent::ApplyRecoil()
{
	ApplyRecoilScaled(1.f);
}

void URecoilComponent::ApplyRecoilScaled(float Scale)
{
	if (Scale <= 0.f) return;

	if (const UWorld* World = GetWorld())
	{
		LastShotTime = World->GetTimeSeconds();
	}

	const float Mult = Scale * (1.f + Ramp);
	Ramp = FMath::Min(Ramp + RampPerShot, RampMax);

	YawSign = -YawSign;
	const float RawYaw = FMath::RandRange(-Data.Horizontal, Data.Horizontal) * 0.55f
	                   + Data.Horizontal * 0.45f * YawSign;

	// ---------- AIM PUNCH ----------
	AimTarget.X += Data.Vertical * AimPunchRatio * Mult;
	AimTarget.Y += RawYaw        * AimPunchRatio * Mult;

	// ---------- VIEW PUNCH ----------
	const float ViewOmega  = Data.CamReturnSpeed * ViewSpringMul;
	const float ViewWeight = (1.f - AimPunchRatio) * ViewPunchScale * Mult;

	const float VPitch = Data.Vertical * ViewWeight;
	const float VYaw   = RawYaw        * ViewWeight;
	const float VRoll  = FMath::RandRange(-1.f, 1.f) * Data.Vertical * 0.30f * Mult;

	ViewVel.Pitch += VPitch * ViewOmega;
	ViewVel.Yaw   += VYaw   * ViewOmega;
	ViewVel.Roll  += VRoll  * ViewOmega;

	// ---------- WEAPON PUNCH ----------
	if (WeaponRoot)
	{
		const float WOmega = Data.WeaponReturnSpeed * WeaponSpringMul;

		if (!FMath::IsNearlyZero(Data.Kickback))
		{
			const float K = Data.Kickback * Mult;
			WeaponLocVel += FVector(
				-K,
				 FMath::RandRange(-0.12f, 0.12f) * K,
				 FMath::RandRange(-0.05f, 0.30f) * K
			) * WOmega;
		}

		if (!FMath::IsNearlyZero(Data.Rot))
		{
			const float R = Data.Rot * Mult;
			WeaponRotVel += FRotator(
				 R,
				 FMath::RandRange(-0.20f, 0.20f) * R,
				 FMath::RandRange(-0.55f, 0.55f) * R
			) * WOmega;
		}
	}

	// ---------- FOV PUNCH ----------
	if (FovPunchPerVertical > 0.f)
	{
		FovVel += (Data.Vertical * FovPunchPerVertical * Mult) * ViewOmega;
	}

	// ---------- HIT STOP ----------
	if (bUseHitStop && Data.Vertical >= HitStopMinVertical)
	{
		const float T = FMath::Clamp(Data.Vertical / 10.f, 0.f, 1.5f);
		BeginHitStop(HitStopMaxDuration * T);
	}
}

// ============================================================================
//  TICK
// ============================================================================
void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DeltaTime <= 0.f) return;

	if (const UWorld* World = GetWorld())
	{
		if (Ramp > 0.f && World->GetTimeSeconds() - LastShotTime > 0.25f)
		{
			Ramp = FMath::FInterpTo(Ramp, 0.f, DeltaTime, 6.f);
		}
	}

	const float Clamped = FMath::Min(DeltaTime, 0.1f);
	const int32 Steps   = FMath::Clamp(FMath::CeilToInt(Clamped * 120.f), 1, 12);
	const float Dt      = Clamped / Steps;

	for (int32 i = 0; i < Steps; ++i)
	{
		StepAim(Dt);
		StepView(Dt);
		StepWeapon(Dt);
		StepFov(Dt);
	}

	ApplyTransforms();
}

void URecoilComponent::StepAim(float Dt)
{
	if (AimTarget.IsNearlyZero() && AimCurrent.IsNearlyZero()) return;

	AimTarget = FMath::Vector2DInterpTo(AimTarget, FVector2D::ZeroVector, Dt, Data.CamReturnSpeed);

	const FVector2D Prev = AimCurrent;
	AimCurrent = FMath::Vector2DInterpTo(AimCurrent, AimTarget, Dt,
	                                     Data.CamReturnSpeed * AimKickSpeedMul);

	FVector2D Delta = AimCurrent - Prev;

	auto Recover = [this](float Cur, float D)
	{
		return ((Cur > 0.f && D < 0.f) || (Cur < 0.f && D > 0.f)) ? D * AimRecoverRatio : D;
	};
	Delta.X = Recover(AimCurrent.X, Delta.X);
	Delta.Y = Recover(AimCurrent.Y, Delta.Y);

	AddControlRotation(Delta.X, Delta.Y);
}

void URecoilComponent::StepView(float Dt)
{
	const float Omega = Data.CamReturnSpeed * ViewSpringMul;
	RecoilSpring::Step(ViewPunch, ViewVel, FRotator::ZeroRotator, Omega, ViewDampRatio, Dt);
}

void URecoilComponent::StepWeapon(float Dt)
{
	if (!WeaponRoot) return;

	const float Omega = Data.WeaponReturnSpeed * WeaponSpringMul;
	RecoilSpring::Step(WeaponLoc, WeaponLocVel, FVector::ZeroVector,   Omega, WeaponDampRatio, Dt);
	RecoilSpring::Step(WeaponRot, WeaponRotVel, FRotator::ZeroRotator, Omega, WeaponDampRatio, Dt);
}

void URecoilComponent::StepFov(float Dt)
{
	if (FovPunchPerVertical <= 0.f) return;

	const float Omega = Data.CamReturnSpeed * ViewSpringMul;
	RecoilSpring::Step(FovPunch, FovVel, 0.f, Omega, 0.7f, Dt);
}

void URecoilComponent::ApplyTransforms()
{
	if (RecoilPivot)
	{
		FRotator PivotRot = ViewPunch;

		if (bPivotAppliesLookPitch)
		{
			if (const APawn* Pawn = Cast<APawn>(GetOwner()))
			{
				PivotRot.Pitch += FRotator::NormalizeAxis(Pawn->GetControlRotation().Pitch);
			}
		}
		RecoilPivot->SetRelativeRotation(PivotRot);
	}

	if (WeaponRoot)
	{
		WeaponRoot->SetRelativeLocation(WeaponOrgLoc + WeaponLoc);
		WeaponRoot->SetRelativeRotation(
			(WeaponOrgRot.Quaternion() * WeaponRot.Quaternion()).Rotator());
	}

	if (Camera && FovPunchPerVertical > 0.f)
	{
		const float BaseFov = Camera->FieldOfView - AppliedFov;
		AppliedFov = FovPunch;
		Camera->SetFieldOfView(BaseFov + AppliedFov);
	}
}

// ============================================================================
//  HELPERS
// ============================================================================
APlayerController* URecoilComponent::GetPC() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			return PC;
		}
	}
	return GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
}

void URecoilComponent::AddControlRotation(float PitchDeg, float YawDeg)
{
	if (FMath::IsNearlyZero(PitchDeg) && FMath::IsNearlyZero(YawDeg)) return;

	APlayerController* PC = GetPC();
	if (!PC) return;

	FRotator Rot = PC->GetControlRotation();
	Rot.Pitch = FRotator::NormalizeAxis(Rot.Pitch) + PitchDeg;
	Rot.Yaw  += YawDeg;

	if (const APlayerCameraManager* CM = PC->PlayerCameraManager)
	{
		Rot.Pitch = FMath::Clamp(Rot.Pitch, CM->ViewPitchMin, CM->ViewPitchMax);
	}

	PC->SetControlRotation(Rot);
}

void URecoilComponent::BeginHitStop(float RealDuration)
{
	UWorld* World = GetWorld();
	if (!World || RealDuration <= 0.f) return;

	const float NowReal = World->GetRealTimeSeconds();
	if (NowReal - LastHitStopRealTime < HitStopMinInterval) return;
	LastHitStopRealTime = NowReal;

	UGameplayStatics::SetGlobalTimeDilation(World, HitStopDilation);

	World->GetTimerManager().SetTimer(
		HitStopHandle, this, &URecoilComponent::EndHitStop,
		FMath::Max(RealDuration * HitStopDilation, 0.001f), false);
}

void URecoilComponent::EndHitStop()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
	}
}