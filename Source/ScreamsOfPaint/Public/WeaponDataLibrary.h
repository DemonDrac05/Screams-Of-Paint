#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponDataTypes.h"
#include "WeaponDataLibrary.generated.h"

UCLASS()
class SCREAMSOFPAINT_API UWeaponDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Data")
	static bool LoadRangeWeaponData(TArray<FRangeWeaponData>& OutData, FString& OutError);

	UFUNCTION(BlueprintPure, Category = "Weapon Data")
	static FString GetWeaponDataPath();
};