// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumberPuzzleCellWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnNumberCellClicked,
    int32,
    CellIndex
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnNumberCellAnimationFinished,
    int32,
    CellIndex,
    bool,
    bWasSuccess
);

UCLASS()
class VR_GAMES_HEADLESS_API UNumberPuzzleCellWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    virtual void NativeTick(
        const FGeometry& MyGeometry,
        float InDeltaTime
    ) override;


    // =====================================================
    // DATA
    // =====================================================

    UPROPERTY(
        BlueprintReadOnly,
        Category = "Number Puzzle|Cell"
    )
    int32 CellIndex = -1;

    UPROPERTY(
        BlueprintReadOnly,
        Category = "Number Puzzle|Cell"
    )
    int32 NumberValue = 0;

    UPROPERTY(
        BlueprintReadOnly,
        Category = "Number Puzzle|Cell"
    )
    bool bActive = false;


    // =====================================================
    // ANIMATION
    // =====================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Animation"
    )
    float SuccessAnimationDuration = 0.32f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Animation"
    )
    float WrongAnimationDuration = 0.30f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Animation"
    )
    float ShakeStrength = 15.0f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Animation"
    )
    float ShakeFrequency = 6.0f;


    // =====================================================
    // EVENTS
    // =====================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Number Puzzle|Events"
    )
    FOnNumberCellClicked OnNumberCellClicked;

    UPROPERTY(
        BlueprintAssignable,
        Category = "Number Puzzle|Events"
    )
    FOnNumberCellAnimationFinished OnNumberCellAnimationFinished;


    // =====================================================
    // API
    // =====================================================

    UFUNCTION(
        BlueprintCallable,
        Category = "Number Puzzle"
    )
    void InitializeCell(
        int32 NewCellIndex
    );

    UFUNCTION(
        BlueprintCallable,
        Category = "Number Puzzle"
    )
    void SetNumber(
        int32 NewNumber
    );

    UFUNCTION(
        BlueprintCallable,
        Category = "Number Puzzle"
    )
    void SetCellActive(
        bool bNewActive
    );

    UFUNCTION(
        BlueprintCallable,
        Category = "Number Puzzle"
    )
    void ResetVisualState();

    UFUNCTION(
        BlueprintCallable,
        Category = "Number Puzzle"
    )
    void PlaySuccessAnimation();

    UFUNCTION(
        BlueprintCallable,
        Category = "Number Puzzle"
    )
    void PlayWrongAnimation();

    UFUNCTION(
        BlueprintPure,
        Category = "Number Puzzle"
    )
    bool IsAnimating() const;


protected:

    /*
     * Se duplichi WBP_ArrowCell:
     *
     * aggiungi un TextBlock chiamato NumberText.
     */
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> NumberText;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> Image_Cell;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Click;


private:

    UFUNCTION()
    void HandleButtonClicked();

    bool bAnimatingSuccess = false;
    bool bAnimatingWrong = false;

    float AnimationTime = 0.0f;
};