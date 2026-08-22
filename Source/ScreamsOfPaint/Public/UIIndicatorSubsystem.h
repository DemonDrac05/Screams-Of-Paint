#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/WorldSubsystem.h"
#include "UIIndicatorSubsystem.generated.h"

UCLASS()
class SCREAMSOFPAINT_API UUIIndicatorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void RegisterEnemy(AActor* Actor);
	
	UFUNCTION(BlueprintCallable)
	void UnregisterEnemy(AActor* Actor);
	
	UFUNCTION(BlueprintCallable)
	void GetIndicators(FVector ViewLocation, float MaxDistance, int32 MaxCount, TArray<AActor*>& OutActors);
private:
	TArray<TWeakObjectPtr<AActor>> RegisteredEnemies;
};
