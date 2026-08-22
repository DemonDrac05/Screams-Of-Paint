#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SOPCharacterBase.generated.h"

UCLASS()
class SCREAMSOFPAINT_API ASOPCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ASOPCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }
	
	UFUNCTION(BlueprintCallable)
	void InitStatsFromTable(UDataTable* Table, FName RowName);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UAbilitySystemComponent> ASC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class USOPAttributeSet> AttributeSet;
	
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
};