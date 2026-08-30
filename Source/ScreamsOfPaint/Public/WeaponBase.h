#pragma once

#include "CoreMinimal.h"
#include "PaintTypes.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnPaintAmmoChanged, EPaintColor, Color, int32, NewAmount);

UCLASS(Abstract)
class SCREAMSOFPAINT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
public:
	AWeaponBase();
	
	// ===== Identity =====
	FORCEINLINE FName GetID() const { return ID; }
	FORCEINLINE float GetDamage() const { return Damage; }
	
	// ===== Paint Ammo =====
	UFUNCTION(BlueprintCallable, Category = "Weapon|Paint")
	bool AddPaint(EPaintColor Color, int32 Quantity);
	
	// ===== Combat Interface =====
	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual bool TryAttack();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Combat")
    void Attack();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Combat")
	void Release();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Combat")
	bool bAllowRapidFire = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Combat")
	bool bHasChargeAttack = false;
protected:
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName ID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Damage;
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Paint")
	EPaintColor CurrentColor = EPaintColor::Red;
	
	static constexpr int32 PaintCost = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Paint")
	TMap<EPaintColor, int32> DefaultPaintAmmo;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon|Paint")
	TMap<EPaintColor, int32> CurrentPaintAmmo;
	
	// ––––– MATERIAL –––––
	// UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category="Weapon|Material")
	// TObjectPtr<UMaterialInterface> Material;
	//
	// UPROPERTY(Transient)
	// TObjectPtr<UMaterialInstanceDynamic> MID;
	//
	// UFUNCTION(BlueprintPure)
	// UMaterialInstanceDynamic* GetWeaponMID() const { return MID; }
	
	// ––––– CAMERA –––––
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UCameraComponent* Camera;
	
	// ––––– HELPERS –––––
	UFUNCTION(BlueprintCallable, Category = "Weapon|Paint")
	bool RemovePaint(EPaintColor Color, int32 Quantity);
	
	UFUNCTION(BlueprintCallable)
	void ResetPaintAmmo(EPaintColor Color);
};
