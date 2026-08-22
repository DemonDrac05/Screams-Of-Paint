// SOPPaintComboSubsystem.cpp
#include "SOPPaintComboSubsystem.h"
#include "Engine/DataTable.h"
#include "ComboTableRow.h"
#include "SingleTableRow.h"

void USOPaintComboSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ComboTable  = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Tables/DT_Combos.DT_Combos"));
    SingleTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Tables/DT_Singles.DT_Singles"));
    
    if (!ComboTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load DT_Combo"));
    }

    if (!SingleTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load DT_Singles"));
    }
}

void USOPaintComboSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

bool USOPaintComboSubsystem::TryGetCombo(const TSet<EPaintColor>& Colors, FComboTableRow& OutCombo) const
{
    if (!ComboTable) return false;

    for (const auto& Pair : ComboTable->GetRowMap())
    {
        const auto* Row = reinterpret_cast<FComboTableRow*>(Pair.Value);
        if (!Row) continue;
        if (Row->RequiredColors.Num() != Colors.Num()) continue;

        bool bMatch = true;
        for (EPaintColor Color : Row->RequiredColors)
            if (!Colors.Contains(Color)) { bMatch = false; break; }   // fix: break, không return

        if (bMatch) { OutCombo = *Row; return true; }
    }
    return false;
}

bool USOPaintComboSubsystem::TryGetSingle(EPaintColor Color, FSingleTableRow& OutSingle) const
{
    if (!SingleTable) return false;

    for (const auto& Pair : SingleTable->GetRowMap())
    {
        const auto* Row = reinterpret_cast<FSingleTableRow*>(Pair.Value);
        if (Row && Row->Color == Color) { OutSingle = *Row; return true; }
    }
    return false;
}