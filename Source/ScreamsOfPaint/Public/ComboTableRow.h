#pragma once
#include "CoreMinimal.h"

#include "PaintTypes.h"

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "ComboTableRow.generated.h"

USTRUCT(BlueprintType)
struct FComboBonusData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) FGameplayTag BonusTag;
    UPROPERTY(EditAnywhere) float Magnitude = 0.f;
};

USTRUCT(BlueprintType)
struct FComboSynergyData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag RequiredStatusTag;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FComboBonusData> Bonuses;
};

USTRUCT(BlueprintType)
struct FComboAoeData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Radius = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<class UGameplayEffect> AoeEffect;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Magnitude = 0.f;
};

USTRUCT(BlueprintType)
struct FComboTableRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSet<EPaintColor> RequiredColors;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<class UGameplayEffect> PrimaryEffect;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Magnitude = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Duration = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float DurationMax = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float ZoneDuration = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FComboAoeData Aoe;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Lifesteal = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag ResidualStatusTag;          // Status.Burn
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<class UNiagaraSystem> Vfx;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FComboSynergyData Synergy;
};