#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaintTypes.h"
#include "ComboTableRow.h"
#include "PaintCoverageComponent.generated.h"

struct FComboTableRow;

struct FPaintBlob
{
    FName       BoneName     = NAME_None;
    FVector     BoneLocalPos = FVector::ZeroVector;
    float       Radius       = 0.f;
    float       TargetRadius = 0.f;
    float       PaintAmount  = 0.f;
    EPaintColor Color        = EPaintColor::None;
    
    int32 Sequence = 0;
};

UCLASS(ClassGroup=Paint, meta=(BlueprintSpawnableComponent))
class SCREAMSOFPAINT_API UPaintCoverageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPaintCoverageComponent() { PrimaryComponentTick.bCanEverTick = true; }

    UFUNCTION(BlueprintCallable, Category="Paint")
    void RegisterHit(FVector WorldHitPos, FName BoneName, EPaintColor Color, float SplatSize);

    UPROPERTY(EditAnywhere, Category="Paint|Blob")
    float MergeDistance = 15.f;

    UPROPERTY(EditAnywhere, Category="Paint|Blob")
    float OverchargeThreshold = 1.f;
    
    UPROPERTY(EditAnywhere, Category="Paint|Blob")
    bool bClearAllColorsOnTrigger = false;

    // UPROPERTY(EditAnywhere, Category="Paint|Visual")
    // float BlobRadiusScale = 60.f;
    //
    // UPROPERTY(EditAnywhere, Category="Paint|Visual")
    // float MaxBlobWorldRadius = 45.f;
    
    UPROPERTY(EditAnywhere, Category="Paint|Visual")
    float FullBodyRadius = 115.f;
    
    UPROPERTY(EditAnywhere, Category="Paint|Visual", meta=(ClampMin="0", ClampMax="1"))
    float InstantSplatFraction = 0.85f;

    UPROPERTY(EditAnywhere, Category="Paint|Visual")
    float SpreadInterpSpeed = 14.f;
    
    UPROPERTY(EditAnywhere, Category="Paint|Visual", meta=(ClampMin="0.3", ClampMax="1.5"))
    float SpreadExponent = 1.f;
    
    UPROPERTY(VisibleAnywhere, Category="Paint|Visual")
    TArray<UMaterialInstanceDynamic*> MeshMIDs;

    UPROPERTY(EditAnywhere, Category="Paint|Debug")
    bool bDebugLogHits = false;
    
    UPROPERTY(EditAnywhere, Category="Paint|Debug")
    bool bEnableComboAndSingle = false;
    
    static constexpr int32 MaxRenderBlobs = 8;
    
protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DT, ELevelTick Tick, FActorComponentTickFunction* Func) override;

private:
    int32 NextSequence = 0;
    
    void TriggerSingle    (EPaintColor Color);
    void TriggerCombo     (const FComboTableRow& Row, float Intensity);
    void TriggerOvercharge(EPaintColor Color);

    void PushBlobsToMaterials();
    void ClearPaintOfColor(EPaintColor Color);
    void ClearAllPaint();

    FLinearColor GetPaintLinearColor(EPaintColor Color) const;

    TArray<FPaintBlob>       Blobs;
    TMap<EPaintColor, float> Coverage;
    TMap<EPaintColor, bool>  bSingleTriggered;
    TMap<EPaintColor, float> Overcharge;
};