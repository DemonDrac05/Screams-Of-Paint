#pragma once
#include "CoreMinimal.h"
#include "PaintTypes.generated.h"

UENUM(BlueprintType)
enum class EPaintColor : uint8
{
    None, Red, Yellow, Blue, White, Black
};

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
    None, Burn, Freeze, Wet, Flashed, Voided, Ensnared, Poisoned, Shocked
};