#include "WeaponBase.h"

#include "Materials/MaterialInstanceDynamic.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DefaultPaintAmmo = {
		{ EPaintColor::Red,		5},
		{ EPaintColor::Yellow,	50},
		{ EPaintColor::Blue,		30},
		{ EPaintColor::White,		5},
		{ EPaintColor::Black,		5}
	};
	
	CurrentPaintAmmo = {
		{ EPaintColor::Red,		5},
		{ EPaintColor::Yellow,	50},
		{ EPaintColor::Blue,		30},
		{ EPaintColor::White,		5},
		{ EPaintColor::Black,		5}
	};
}

void AWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// if (!MID && Material) MID = UMaterialInstanceDynamic::Create(Material, this);
}

bool AWeaponBase::AddPaint(EPaintColor Color, int32 Quantity)
{
	int32* Current = CurrentPaintAmmo.Find(Color);
	if (!Current) return false;

	CurrentPaintAmmo[Color] = FMath::Max(*Current + Quantity, DefaultPaintAmmo[Color]);
	return true;
}

bool AWeaponBase::RemovePaint(EPaintColor Color, int32 Quantity)
{
	int32* Current = CurrentPaintAmmo.Find(Color);
	if (!Current) return false;

	CurrentPaintAmmo[Color] = FMath::Max(0, *Current - Quantity);
	return true;
}

void AWeaponBase::ResetPaintAmmo(EPaintColor Color)
{
	if (const int32* DefaultCurrent = DefaultPaintAmmo.Find(Color))
	{
		int32& Current = CurrentPaintAmmo.FindOrAdd(Color);
		Current = *DefaultCurrent;
	}
}

bool AWeaponBase::TryAttack() { return true; }

void AWeaponBase::Attack_Implementation(){ }

void AWeaponBase::Release_Implementation(){ }