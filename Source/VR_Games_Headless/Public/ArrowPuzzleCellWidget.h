// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArrowPuzzleCellWidget.generated.h"

class UButton;
class UImage;
class UTexture2D;

UENUM(BlueprintType)
enum class EArrowDirection : uint8
{
    Up      UMETA(DisplayName = "Up"),
    Right   UMETA(DisplayName = "Right"),
    Down    UMETA(DisplayName = "Down"),
    Left    UMETA(DisplayName = "Left")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnArrowCellClicked,
    int32,
    CellIndex
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnArrowCellAnimationFinished,
    int32,
    CellIndex,
    bool,
    bWasRemoval
);

UCLASS()
class VR_GAMES_HEADLESS_API UArrowPuzzleCellWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    virtual void NativeTick(
        const FGeometry& MyGeometry,
        float InDeltaTime
    ) override;

    // =========================================================
    // CELL DATA
    // =========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Cell"
    )
    int32 GridRow = 0;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Cell"
    )
    int32 GridColumn = 0;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Cell"
    )
    EArrowDirection StartDirection = EArrowDirection::Up;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Cell"
    )
    bool bStartsActive = true;

    UPROPERTY(
        BlueprintReadOnly,
        Category = "Arrow Puzzle|Cell"
    )
    int32 CellIndex = -1;

    UPROPERTY(
        BlueprintReadOnly,
        Category = "Arrow Puzzle|Cell"
    )
    bool bActive = true;

    UPROPERTY(
        BlueprintReadOnly,
        Category = "Arrow Puzzle|Cell"
    )
    bool bAvailable = false;

    // =========================================================
    // VISUAL
    // =========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Visual"
    )
    TObjectPtr<UTexture2D> NormalCellTexture;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Visual"
    )
    TObjectPtr<UTexture2D> AvailableCellTexture;

    // =========================================================
    // ANIMATION
    // =========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Animation",
        meta = (ClampMin = "0.05")
    )
    float RemoveAnimationDuration = 0.40f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Animation",
        meta = (ClampMin = "0.1")
    )
    float RemoveDistanceMultiplier = 2.5f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Animation",
        meta = (ClampMin = "0.05")
    )
    float BlockedAnimationDuration = 0.35f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Animation"
    )
    float ShakeStrength = 16.0f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Animation"
    )
    float ShakeFrequency = 6.0f;

    // =========================================================
    // EVENTS
    // =========================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Arrow Puzzle|Events"
    )
    FOnArrowCellClicked OnArrowCellClicked;

    /*
     * NON chiamarlo OnAnimationFinished.
     * UUserWidget possiede gi  quel nome.
     */
    UPROPERTY(
        BlueprintAssignable,
        Category = "Arrow Puzzle|Events"
    )
    FOnArrowCellAnimationFinished OnCellAnimationFinished;

    // =========================================================
    // API
    // =========================================================

    UFUNCTION(BlueprintCallable, Category = "Arrow Puzzle")
    void InitializeCell(int32 NewCellIndex);

    UFUNCTION(BlueprintCallable, Category = "Arrow Puzzle")
    void ResetCell();

    UFUNCTION(BlueprintCallable, Category = "Arrow Puzzle")
    void SetAvailable(bool bNewAvailable);

    UFUNCTION(BlueprintCallable, Category = "Arrow Puzzle")
    void BeginRemoveAnimation();

    UFUNCTION(BlueprintCallable, Category = "Arrow Puzzle")
    void BeginBlockedAnimation();

    UFUNCTION(BlueprintPure, Category = "Arrow Puzzle")
    bool IsAnimating() const;

protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Image_Cell;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Image_Arrow;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Click;

private:

    UFUNCTION()
    void HandleButtonClicked();

    void UpdateArrowRotation();

    float DirectionToAngle(
        EArrowDirection Direction
    ) const;

    FVector2D DirectionToVector(
        EArrowDirection Direction
    ) const;

    bool bAnimatingRemoval = false;
    bool bAnimatingBlocked = false;

    float AnimationTime = 0.0f;
};