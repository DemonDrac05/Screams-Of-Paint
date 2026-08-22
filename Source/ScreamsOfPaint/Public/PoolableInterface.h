#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableInterface.generated.h"

UINTERFACE()
class SCREAMSOFPAINT_API UPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

class IPoolableInterface
{
	GENERATED_BODY()
public:
	void OnSpawnFromPool();
	void OnReturnToPool();
};
