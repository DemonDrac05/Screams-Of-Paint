#include "WeaponDataLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "JsonObjectConverter.h"

FString UWeaponDataLibrary::GetWeaponDataPath()
{
	return FPaths::ProjectSavedDir() / TEXT("WeaponConfig/range_weapons_data.json");
}

bool UWeaponDataLibrary::LoadRangeWeaponData(TArray<FRangeWeaponData>& OutData, FString& OutError)
{
	const FString Path = GetWeaponDataPath();

	FString JsonStr;
	if (!FFileHelper::LoadFileToString(JsonStr, *Path))
	{
		OutError = FString::Printf(TEXT("Khong doc duoc file: %s"), *Path);
		return false;
	}

	OutData.Reset();
	if (!FJsonObjectConverter::JsonArrayStringToUStruct(JsonStr, &OutData, 0, 0))
	{
		OutError = TEXT("Parse JSON that bai - kiem tra syntax (thieu dau phay?)");
		return false;
	}

	OutError.Empty();
	return true;
}