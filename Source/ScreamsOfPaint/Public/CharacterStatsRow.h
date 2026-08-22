#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "CharacterStatsRow.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStatsRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MovementSpeed = 600.f;
};
