// AvantGravePaintAttributeSet.cpp
#include "SOPAttributeSet.h"
#include "Net/UnrealNetwork.h"

void USOPAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(USOPAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USOPAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USOPAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}
void USOPAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{ GAMEPLAYATTRIBUTE_REPNOTIFY(USOPAttributeSet, Health, OldValue); }
void USOPAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{ GAMEPLAYATTRIBUTE_REPNOTIFY(USOPAttributeSet, MaxHealth, OldValue); }
void USOPAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{ GAMEPLAYATTRIBUTE_REPNOTIFY(USOPAttributeSet, MovementSpeed, OldValue); }