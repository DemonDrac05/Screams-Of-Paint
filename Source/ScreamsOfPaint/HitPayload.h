#pragma once

#include "CoreMinimal.h"
#include "PaintTypes.h"
#include "UObject/Interface.h"
#include "HitPayload.generated.h"

USTRUCT(BlueprintType)
struct FHitPayload
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly) EPaintColor Color = EPaintColor::Red;
	UPROPERTY(BlueprintReadOnly) float Damage = 0.f;
	UPROPERTY(BlueprintReadOnly) FVector Direction = FVector::Zero();
	UPROPERTY(BlueprintReadOnly) FVector ImpactPoint = FVector::Zero();
	UPROPERTY(BlueprintReadOnly) FVector ImpactNormal = FVector::Zero();
	UPROPERTY(BlueprintReadOnly) float Speed = 0.f;
	UPROPERTY(BlueprintReadOnly) float Lifetime = 0.f;
};

UINTERFACE(BlueprintType)
class UHittable : public UInterface { GENERATED_BODY() };

class SCREAMSOFPAINT_API IHittable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void ReceiveHit(const FHitPayload& Payload);
};
