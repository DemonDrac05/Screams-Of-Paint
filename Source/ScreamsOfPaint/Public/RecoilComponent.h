#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDataTypes.h"
#include "RecoilComponent.generated.h"

UCLASS(ClassGroup =(Weapon), meta=(BlueprintSpawnableComponent))
class SCREAMSOFPAINT_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	URecoilComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void Setup(USceneComponent* InRecoilPivot, USceneComponent* InWeaponRoot);
	
	void RefreshData(const FRecoilData& InData);
	
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ApplyRecoil();
protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, 
								FActorComponentTickFunction* ThisTickFunction) override;
private:
	UPROPERTY() USceneComponent* RecoilPivot = nullptr;
	UPROPERTY() USceneComponent* WeaponRoot = nullptr;
	
	FRecoilData Data;
	FRotator TargetRecoil	 = FRotator::ZeroRotator;
	FRotator CurrentRecoil	 = FRotator::ZeroRotator;
	FVector WeaponOrgLoc	 = FVector::ZeroVector;
	FRotator WeaponOrgRot	 = FRotator::ZeroRotator;
};
