#pragma once
#include "CoreMinimal.h"
#include "WeaponDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FRecoilData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Vertical = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Horizontal = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Kickback = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Rot = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float CamReturnSpeed = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float WeaponReturnSpeed = 5.f;
};

USTRUCT(BlueprintType)
struct FRangeWeaponSpecialData
{
    GENERATED_BODY()

    // Flower gun
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ChargeTimeMin = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ChargeTimeMax = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float DamageMin = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float DamageMax = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SplatRadiusMin = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SplatRadiusMax = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShockBuildupMin = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShockBuildupMax = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShockThreshold = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShockDecayPerSec = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunDuration = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ChainRadius = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ChainMaxHops = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ChainFalloff = 0.f;

    // Crossbow
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float DrawTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 PierceCount = 0;

    // Shotgun
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ShotsBeforeClog = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FlushHoldTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FlushDrainMultiplier = 0.f;

    // Fragment gun
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SpinUpTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RecoilRampPerShot = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RecoilRampMax = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FinalShotDamage = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 FinalShotPierce = 0;

    // Heart gun
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 StageCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StageInterval = 0.f;
};

USTRUCT(BlueprintType)
struct FProjectileSpecialData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RangeMin = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RangeMax = 0.f;
};

USTRUCT(BlueprintType)
struct FProjectileData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Speed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Range = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float LifeTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FProjectileSpecialData ProjectileSpecial;
};

USTRUCT(BlueprintType)
struct FRangeWeaponData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LockedColor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Damage = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HitscanRange = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Spread = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FireRate = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Magazine = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ReloadTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 PelletCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FRecoilData Recoil;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FRangeWeaponSpecialData RangeWeaponSpecial;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FProjectileData Projectile;
};