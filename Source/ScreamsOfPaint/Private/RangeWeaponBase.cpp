#include "RangeWeaponBase.h"

#include "DrawDebugHelpers.h"
#include "HitPayload.h"
#include "TimerManager.h"

#include "PaintTypes.h"
#include "RecoilComponent.h"
#include "PaintCoverageComponent.h"
#include "WeaponControllerComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

ARangeWeaponBase::ARangeWeaponBase()
{
	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(RootComponent);
}

void ARangeWeaponBase::SetData(const FRangeWeaponData& InData)
{
	WeaponData = InData;
	
	WeaponData.HitscanRange		*= 100.f;
	WeaponData.Projectile.Speed *= 100.f;
	WeaponData.Projectile.Range *= 100.f;
	WeaponData.Recoil.Kickback	*= 100.f;
	
	Damage = WeaponData.Damage;
	
	if (const UEnum* Enum = StaticEnum<EPaintColor>())
	{
		const int64 Value = Enum->GetValueByNameString(WeaponData.LockedColor);
		if (Value != INDEX_NONE) CurrentColor = static_cast<EPaintColor>(Value);
	}
	
	if (RecoilComp)
	{
		RecoilComp->RefreshData(WeaponData.Recoil);
		RecoilComp->ClearRecoil();
	}
}

float ARangeWeaponBase::GetRecoilScale_Implementation()
{
	return 1.f;
}

void ARangeWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = GetParentActor();

	if (OwningCharacter)
	{
		if (!Camera)     Camera     = OwningCharacter->FindComponentByClass<UCameraComponent>();
		if (!RecoilComp) RecoilComp = OwningCharacter->FindComponentByClass<URecoilComponent>();
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] Parent=%s Camera=%s Recoil=%s"),
		*GetName(),
		OwningCharacter ? *OwningCharacter->GetName() : TEXT("NULL"),
		Camera ? TEXT("OK") : TEXT("NULL"),
		RecoilComp ? TEXT("OK") : TEXT("NULL"));
	
	if (RecoilComp)
	{
		RecoilComp->RefreshData(WeaponData.Recoil);

		if (WeaponData.Id == TEXT("fragment_gun"))
			RecoilComp->SetRamp(0.05f, 1.6f);
		else
			RecoilComp->SetRamp(0.f, 0.f);
	}
	
	if (!UsesMagazine()) return;
	CurrentShot = MaxShotsPerMagazine;
}

void ARangeWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool ARangeWeaponBase::CanFire()
{
	if (WeaponData.FireRate <= 0.f) return false;

	const double Now      = GetWorld()->GetTimeSeconds();
	const double Interval = 1.0 / WeaponData.FireRate;
	const bool   bCan     = (Now - LastFireTime) >= Interval;

	UE_LOG(LogTemp, Warning, TEXT("[%s] Now=%.2f Last=%.2f Interval=%.2f -> %s"),
		*GetName(), Now, LastFireTime, Interval, bCan ? TEXT("FIRE") : TEXT("wait"));

	if (bCan) LastFireTime = Now;
	return bCan;
}

bool ARangeWeaponBase::TryAttack()
{
	if (bHasFinalShot){ FireFinalShot(); return false; }
	
	if (!HasEnoughMagazine()) return false;
	if (!CanFire()) return false;
	
	Attack_Implementation();
	return true;
}

void ARangeWeaponBase::Attack_Implementation()
{
	ProcessFire();
	
	if (!UsesMagazine()) return;
	
	--CurrentShot;
	if (CurrentShot == 0)
	{
		bAllowRapidFire = false;
		PrepareFinalShot();
	}
}

void ARangeWeaponBase::ReloadMagazine()
{
	CurrentShot = MaxShotsPerMagazine;
}

void ARangeWeaponBase::ProcessFire_Implementation()
{
	CalculateBulletPath();
	RemovePaint(CurrentColor, PaintCost);
	if (RecoilComp) RecoilComp->ApplyRecoilScaled(GetRecoilScale());
}

void ARangeWeaponBase::CalculateBulletPath()
{
    if (!Camera) return;

    const FVector CamLoc     = Camera->GetComponentLocation();
    const FVector CamForward = Camera->GetForwardVector();
    const FVector CamRight   = Camera->GetRightVector();
    const FVector CamUp      = Camera->GetUpVector();

	const FVector TracerStart = Muzzle ? Muzzle->GetComponentLocation() : CamLoc;
    for (int32 i = 0; i < WeaponData.PelletCount; i++)
    {
        FVector Direction = CamForward;
        if (const float Spread = WeaponData.Spread; Spread > 0.f)
        {
            Direction += CamRight * FMath::RandRange(-Spread, Spread)
                       + CamUp    * FMath::RandRange(-Spread, Spread);
        }
        Direction.Normalize();

        const FVector Start = CamLoc;
        const FVector End   = CamLoc + Direction * WeaponData.HitscanRange;

        FCollisionQueryParams Params;
    	Params.AddIgnoredActor(this);
    	if (OwningCharacter) Params.AddIgnoredActor(OwningCharacter);

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
        	if (AActor* HitActor = Hit.GetActor())
        	{
        		if (HitActor->Implements<UHittable>())
        		{
        			FHitPayload Payload;
        			Payload.Color        = CurrentColor;
        			Payload.Damage       = Damage;
        			Payload.Direction    = Direction;
        			Payload.ImpactPoint  = Hit.ImpactPoint;
        			Payload.ImpactNormal = Hit.ImpactNormal;
        			IHittable::Execute_ReceiveHit(HitActor, Payload);
        		}

        		if (UPaintCoverageComponent* PaintComp =
						HitActor->FindComponentByClass<UPaintCoverageComponent>())
        		{
        			PaintComp->RegisterHit(Hit.ImpactPoint, Hit.BoneName, CurrentColor, 0.25f);
        			
        			if (GEngine)
        			{
        				GEngine->AddOnScreenDebugMessage(
							-1,
							5.f,
							FColor::Yellow,
							FString::Printf(TEXT("Hit impact point: %s | Hit bone name: %s"),
								*Hit.ImpactPoint.ToString(),
								*Hit.BoneName.ToString()
							)
						);
        			}
        		}
        		else
        		{
        			if (GEngine)
        			{
        				GEngine->AddOnScreenDebugMessage(
							-1,                     
							5.f,                    
							FColor::Red,
							TEXT("Not found comp!")
						);
        			}
        		}
        	}
        	
            DrawDebugLine(GetWorld(), TracerStart, Hit.ImpactPoint, FColor::Red, false, 1.f);
        }
        else
        {
            DrawDebugLine(GetWorld(), TracerStart, End, FColor::Yellow, false, 1.f);
        }
    }
}

FVector ARangeWeaponBase::FindTargetPoint() const
{
    if (!Camera) return FVector::ForwardVector;

    constexpr float MaxRange = 100000.f;
    const FVector Start = Camera->GetComponentLocation();
    const FVector End   = Start + Camera->GetForwardVector() * MaxRange;

    FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (OwningCharacter) Params.AddIgnoredActor(OwningCharacter);

    FVector TargetPoint = End;
    if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        TargetPoint = Hit.ImpactPoint;

    return (TargetPoint - Muzzle->GetComponentLocation()).GetSafeNormal();
}

bool ARangeWeaponBase::HasEnoughMagazine()
{
	return CurrentPaintAmmo[CurrentColor] > 0;
}

void ARangeWeaponBase::Release_Implementation()
{
	if (RecoilComp) RecoilComp->ResetRamp();
}