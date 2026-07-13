// SOPPaintComboSubsystem.h
#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PaintTypes.h"
#include "SOPPaintComboSubsystem.generated.h"

struct FComboTableRow;
struct FSingleTableRow;

UCLASS()
class SCREAMSOFPAINT_API USOPaintComboSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool TryGetCombo(const TSet<EPaintColor>& Colors, FComboTableRow& OutCombo) const;
	bool TryGetSingle(EPaintColor Color, FSingleTableRow& OutSingle) const;

private:
	UPROPERTY() TObjectPtr<class UDataTable> ComboTable;
	UPROPERTY() TObjectPtr<class UDataTable> SingleTable;
};