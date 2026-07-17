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
	/** Đọc JSON từ <Project>/Saved/WeaponConfig/range_weapons_data.json */
	UFUNCTION(BlueprintCallable, Category = "Weapon Data")
	static bool LoadRangeWeaponData(TArray<FRangeWeaponData>& OutData, FString& OutError);

	/** Đường dẫn file, để in ra màn hình cho GD biết sửa file nào */
	UFUNCTION(BlueprintPure, Category = "Weapon Data")
	static FString GetWeaponDataPath();
};