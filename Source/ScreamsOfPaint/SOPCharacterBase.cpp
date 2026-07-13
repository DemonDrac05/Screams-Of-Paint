#include "SOPCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "CharacterStatsRow.h"
#include "SOPAttributeSet.h"

ASOPCharacterBase::ASOPCharacterBase()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<USOPAttributeSet>(TEXT("AttributeSet"));
}

void ASOPCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (ASC) ASC->InitAbilityActorInfo(this, this);
}

void ASOPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASOPCharacterBase::InitStatsFromTable(UDataTable* Table, FName RowName)
{
	if (!Table || !AttributeSet) return;
	FCharacterStatsRow* Row = Table->FindRow<FCharacterStatsRow>(RowName, TEXT("InitStats"));
	if (!Row) return;

	AttributeSet->InitMaxHealth(Row->MaxHealth);
	AttributeSet->InitHealth(Row->MaxHealth);
	AttributeSet->InitMovementSpeed(Row->MovementSpeed);
}
