#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OddPuzzleCellWidget.generated.h"

class UBorder;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnOddCellClicked,
    int32,
    CellIndex
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnOddCellAnimationFinished,
    int32,
    CellIndex,
    bool,
    bWasSuccess
);

UCLASS()
class VR_GAMES_HEADLESS_API UOddPuzzleCellWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    virtual void NativeTick(
        const FGeometry& MyGeometry,
        float InDeltaTime
    ) override;

    // =========================================================
    // STATE
    // =========================================================

    UPROPERTY(BlueprintReadOnly, Category = "Odd Puzzle|Cell")
    int32 CellIndex = -1;

    UPROPERTY(BlueprintReadOnly, Category = "Odd Puzzle|Cell")
    bool bActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Odd Puzzle|Cell")
    bool bIsOddCell = false;

    // =========================================================
    // ANIMATION
    // =========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Animation"
    )
    float SuccessAnimationDuration = 0.35f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Animation"
    )
    float WrongAnimationDuration = 0.30f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Animation"
    )
    float ShakeStrength = 15.0f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Animation"
    )
    float ShakeFrequency = 6.0f;

    // =========================================================
    // EVENTS
    // =========================================================

    UPROPERTY(BlueprintAssignable, Category = "Odd Puzzle|Events")
    FOnOddCellClicked OnOddCellClicked;

    UPROPERTY(BlueprintAssignable, Category = "Odd Puzzle|Events")
    FOnOddCellAnimationFinished OnOddCellAnimationFinished;

    // =========================================================
    // API
    // =========================================================

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void InitializeCell(int32 NewCellIndex);

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void ConfigureCell(
        const FLinearColor& NewColor,
        bool bNewOddCell,
        bool bNewActive
    );

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void SetCellColor(
        const FLinearColor& NewColor
    );

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void SetCellActive(
        bool bNewActive
    );

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void ResetVisualState();

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void PlaySuccessAnimation();

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void PlayWrongAnimation();

    UFUNCTION(BlueprintPure, Category = "Odd Puzzle")
    bool IsAnimating() const;

protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> Border_Color;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Click;

private:

    UFUNCTION()
    void HandleButtonClicked();

    bool bAnimatingSuccess = false;
    bool bAnimatingWrong = false;

    float AnimationTime = 0.0f;
};