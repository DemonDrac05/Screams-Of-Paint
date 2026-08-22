#pragma once

#include "CoreMinimal.h"
#include "PoolableInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyPoolSubsystem.generated.h"

UCLASS()
class SCREAMSOFPAINT_API UEnemyPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	
private:
	TArray<AActor*> EnemyPool;
};
