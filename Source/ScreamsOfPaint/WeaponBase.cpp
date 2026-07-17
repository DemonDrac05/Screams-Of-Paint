#include "WeaponBase.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DefaultPaintAmmo = {
		{ EPaintColor::Red,		5},
		{ EPaintColor::Yellow,	50},
		{ EPaintColor::Blue,		5},
		{ EPaintColor::White,		5},
		{ EPaintColor::Black,		5}
	};
	
	PaintAmmo = {
		{ EPaintColor::Red,		5},
		{ EPaintColor::Yellow,	50},
		{ EPaintColor::Blue,		5},
		{ EPaintColor::White,		5},
		{ EPaintColor::Black,		5}
	};
}

bool AWeaponBase::AddPaint(EPaintColor Color, int32 Quantity)
{
	int32* Current = PaintAmmo.Find(Color);
	if (!Current) return false;

	PaintAmmo[Color] = FMath::Max(*Current + Quantity, DefaultPaintAmmo[Color]);
	return true;
}

bool AWeaponBase::RemovePainṭ̣̣̣(EPaintColor Color, int32 Quantity)
{
	int32* Current = PaintAmmo.Find(Color);
	if (!Current) return false;

	PaintAmmo[Color] = FMath::Max(0, *Current - Quantity);
	return true;
}

void AWeaponBase::ResetPaintAmmo(EPaintColor Color)
{
	if (const int32* DefaultCurrent = DefaultPaintAmmo.Find(Color))
	{
		int32& Current = PaintAmmo.FindOrAdd(Color);
		Current = *DefaultCurrent;
	}
}

bool AWeaponBase::HasPaint(EPaintColor Color) const
{
	const int32* Current = PaintAmmo.Find(Color);
	return Current && *Current > 0;
}
