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
	virtual void Attack() {}
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual void Release() {}
	
	virtual bool AllowRapidFire() const { return false; }
	
	// ===== Events =====
	// UPROPERTY(BlueprintAssignable, Category = "Weapon|Paint")
	
	
protected:
	
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
	TMap<EPaintColor, int32> PaintAmmo;
	
	// ===== Camera =====
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UCameraComponent* Camera;
	
	// ===== Functions =====
	UFUNCTION(BlueprintCallable, Category = "Weapon|Paint")
	bool RemovePaint(EPaintColor Color, int32 Quantity);
	
	UFUNCTION(BlueprintCallable)
	void ResetPaintAmmo(EPaintColor Color);
	
	UFUNCTION(BlueprintCallable)
	bool HasPaint(EPaintColor Color) const;
	
	UFUNCTION(BlueprintCallable)
	void InitializeCamera(UCameraComponent* InCamera);
};
