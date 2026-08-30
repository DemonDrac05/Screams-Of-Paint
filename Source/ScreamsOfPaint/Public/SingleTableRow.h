#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "PaintTypes.h"
#include "Templates/SubclassOf.h"
#include "SingleTableRow.generated.h"

USTRUCT(BlueprintType)
struct FSingleTableRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EPaintColor Color = EPaintColor::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag AppliedStatusTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<class UGameplayEffect> Effect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Magnitude = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Duration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag ResidualStatusTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ResidualDuration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bHasOvercharge = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<class UGameplayEffect> OverchargeEffect;
};