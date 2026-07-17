#include "RangeWeaponBase.h"
#include "WeaponDataLibrary.h"

void ARangeWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<FRangeWeaponData> AllDatas;
	FString Error;
	
	if (UWeaponDataLibrary::LoadRangeWeaponData(AllDatas, Error))
	{
		for (const FRangeWeaponData& Data : AllDatas)
		{
			if (Data.Id != GetID().ToString()) continue;
			this->WeaponData = Data;
		}
	}
}

void ARangeWeaponBase::CalculateBulletPath()
{
	
}
