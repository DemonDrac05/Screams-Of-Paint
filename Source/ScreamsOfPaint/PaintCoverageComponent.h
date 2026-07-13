// PaintCoverageComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaintTypes.h"
#include "ComboTableRow.h"
#include "PaintCoverageComponent.generated.h"

struct FComboTableRow;

// ─── Blob: vết sơn tại 1 vị trí local trên enemy ────────────────────────────
struct FPaintBlob
{
    FName       BoneName     = NAME_None;
    FVector     BoneLocalPos = FVector::ZeroVector;
    float       Radius       = 0.f;   // hiển thị (interp mượt)
    float       TargetRadius = 0.f;   // gameplay (cộng dồn SplatSize)
    EPaintColor Color        = EPaintColor::None;
};

// ─── Component: gắn lên BP_Enemy ────────────────────────────────────────────
// Render bằng WORLD-SPACE SPHERE MASK: mỗi tick đẩy tối đa MaxRenderBlobs
// blob (world pos + radius + màu) vào material parameters. Material tự tính
// khoảng cách pixel→blob để tô sơn. KHÔNG UV, KHÔNG RenderTarget, không seam,
// không mirror, không cần calibrate.
UCLASS(ClassGroup=Paint, meta=(BlueprintSpawnableComponent))
class SCREAMSOFPAINT_API UPaintCoverageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPaintCoverageComponent() { PrimaryComponentTick.bCanEverTick = true; }

    UFUNCTION(BlueprintCallable, Category="Paint")
    void RegisterHit(FVector WorldHitPos, FName BoneName,
                     EPaintColor Color, float SplatSize);

    // ── Tuning ───────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category="Paint|Blob")
    float MergeDistance = 15.f;        // cm — blob cùng màu gần nhau thì merge

    UPROPERTY(EditAnywhere, Category="Paint|Blob")
    float OverchargeThreshold = 1.f;

    /** SplatSize (đơn vị gameplay) → bán kính world (cm).
        VD SplatSize 0.1/viên × Scale 60 = vết ~6cm, bắn dồn thì to dần. */
    UPROPERTY(EditAnywhere, Category="Paint|Visual")
    float BlobRadiusScale = 60.f;

    /** Bán kính world tối đa của 1 blob (cm) */
    UPROPERTY(EditAnywhere, Category="Paint|Visual")
    float MaxBlobWorldRadius = 45.f;

    /** Số blob tối đa gửi vào material — PHẢI khớp số param trong material */
    static constexpr int32 MaxRenderBlobs = 8;

    UPROPERTY(EditAnywhere, Category="Paint|Debug")
    bool bDebugLogHits = false;

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> MeshMIDs;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DT, ELevelTick Tick,
                               FActorComponentTickFunction* Func) override;

private:
    void TriggerSingle    (EPaintColor Color);
    void TriggerCombo     (const FComboTableRow& Row, float Intensity);
    void TriggerOvercharge(EPaintColor Color);

    void PushBlobsToMaterials();   // blob → material params, gọi mỗi tick
    void ClearAllPaint();

    FLinearColor GetPaintLinearColor(EPaintColor Color) const;

    TArray<FPaintBlob>       Blobs;
    TMap<EPaintColor, float> Coverage;
    TMap<EPaintColor, bool>  bSingleTriggered;
    TMap<EPaintColor, float> Overcharge;
};