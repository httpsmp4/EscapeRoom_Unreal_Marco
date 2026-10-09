#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickTapCellWidget.generated.h"

class UBorder;
class UButton;
class UImage;


UENUM(BlueprintType)
enum class EQuickTapCellAnimationType : uint8
{
    None        UMETA(DisplayName = "None"),
    Success     UMETA(DisplayName = "Success"),
    Wrong       UMETA(DisplayName = "Wrong"),
    Miss        UMETA(DisplayName = "Miss")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnQuickTapCellClicked,
    int32,
    CellIndex
);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnQuickTapCellAnimationFinished,
    int32,
    CellIndex,
    EQuickTapCellAnimationType,
    AnimationType
);


UCLASS()
class VR_GAMES_HEADLESS_API UQuickTapCellWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    virtual void NativeTick(
        const FGeometry& MyGeometry,
        float InDeltaTime
    ) override;


    // ========================================================
    // STATE
    // ========================================================

    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap|Cell"
    )
    int32 CellIndex = -1;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap|Cell"
    )
    bool bActive = false;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap|Cell"
    )
    bool bIsTarget = false;


    // ========================================================
    // ANIMATION
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Animation"
    )
    float SuccessAnimationDuration = 0.24f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Animation"
    )
    float WrongAnimationDuration = 0.28f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Animation"
    )
    float MissAnimationDuration = 0.30f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Animation"
    )
    float ShakeStrength = 16.0f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Animation"
    )
    float ShakeFrequency = 6.0f;


    // ========================================================
    // EVENTS
    // ========================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Quick Tap|Events"
    )
    FOnQuickTapCellClicked OnQuickTapCellClicked;


    UPROPERTY(
        BlueprintAssignable,
        Category = "Quick Tap|Events"
    )
    FOnQuickTapCellAnimationFinished
        OnQuickTapCellAnimationFinished;


    // ========================================================
    // API
    // ========================================================

    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void InitializeCell(
        int32 NewCellIndex
    );


    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void ConfigureCell(
        const FLinearColor& NewColor,
        bool bNewIsTarget,
        bool bNewActive
    );


    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void SetCellColor(
        const FLinearColor& NewColor
    );


    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void SetCellActive(
        bool bNewActive
    );


    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void ResetVisualState();


    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void PlaySuccessAnimation();


    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void PlayWrongAnimation();


    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void PlayMissAnimation();


    UFUNCTION(
        BlueprintPure,
        Category = "Quick Tap"
    )
    bool IsAnimating() const;


protected:

    // ========================================================
    // UI
    // ========================================================

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> Border_Color;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> Image_Frame;


    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Click;


private:

    UFUNCTION()
    void HandleButtonClicked();


    EQuickTapCellAnimationType ActiveAnimation =
        EQuickTapCellAnimationType::None;


    float AnimationTime = 0.0f;
};