#include "UIIndicatorSubsystem.h"

void UUIIndicatorSubsystem::RegisterEnemy(AActor* Actor)
{
	if (RegisteredEnemies.Contains(Actor)) return;
	
	RegisteredEnemies.Add(Actor);
}

void UUIIndicatorSubsystem::UnregisterEnemy(AActor* Actor)
{
	if (!RegisteredEnemies.Contains(Actor)) return;
	
	RegisteredEnemies.Remove(Actor);
}

void UUIIndicatorSubsystem::GetIndicators(FVector ViewLocation, float MaxDistance, 
											int32 MaxCount, TArray<AActor*>& OutActors)
{
	OutActors.Reset();
	const float MaxDistSquared = MaxDistance * MaxDistance;
	
	for (int32 i = RegisteredEnemies.Num() - 1; i >= 0; --i)
	{
		AActor* A = RegisteredEnemies[i].Get();
		if (!IsValid(A)) { RegisteredEnemies.RemoveAtSwap(i); continue; }
		if (FVector::DistSquared(A->GetActorLocation(), ViewLocation) > MaxDistSquared) continue;
		OutActors.Add(A);
	}
	
	OutActors.Sort([ViewLocation](const AActor& A, const AActor& B)
	{
		return FVector::DistSquared(A.GetActorLocation(), ViewLocation)
			 < FVector::DistSquared(B.GetActorLocation(), ViewLocation);
	});
	
	if (OutActors.Num() > MaxCount) OutActors.SetNum(MaxCount);
}
