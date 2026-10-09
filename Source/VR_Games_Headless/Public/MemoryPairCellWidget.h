
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MemoryPairCellWidget.generated.h"

    class UButton;
    class UImage;
    class UTexture2D;


    UENUM(BlueprintType)
        enum class EMemoryCardState : uint8
    {
        Hidden      UMETA(DisplayName = "Hidden"),
        Revealed    UMETA(DisplayName = "Revealed"),
        Matched     UMETA(DisplayName = "Matched")
    };


    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnMemoryCardClicked,
        int32,
        CellIndex
    );


    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnMemoryCardFlipFinished,
        int32,
        CellIndex,
        bool,
        bIsFaceUp
    );


    UCLASS()
        class VR_GAMES_HEADLESS_API UMemoryPairCellWidget : public UUserWidget
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
            Category = "Memory Pairs|Cell"
        )
        int32 CellIndex = -1;


        UPROPERTY(
            BlueprintReadOnly,
            Category = "Memory Pairs|Cell"
        )
        int32 PairId = -1;


        UPROPERTY(
            BlueprintReadOnly,
            Category = "Memory Pairs|Cell"
        )
        EMemoryCardState CardState =
            EMemoryCardState::Hidden;


        // ========================================================
        // ANIMATION
        // ========================================================

        UPROPERTY(
            EditAnywhere,
            BlueprintReadWrite,
            Category = "Memory Pairs|Animation"
        )
        float FlipDuration = 0.30f;


        UPROPERTY(
            EditAnywhere,
            BlueprintReadWrite,
            Category = "Memory Pairs|Animation"
        )
        float MatchPulseDuration = 0.30f;


        // ========================================================
        // EVENTS
        // ========================================================

        UPROPERTY(
            BlueprintAssignable,
            Category = "Memory Pairs|Events"
        )
        FOnMemoryCardClicked OnMemoryCardClicked;


        UPROPERTY(
            BlueprintAssignable,
            Category = "Memory Pairs|Events"
        )
        FOnMemoryCardFlipFinished OnMemoryCardFlipFinished;


        // ========================================================
        // API
        // ========================================================

        UFUNCTION(BlueprintCallable, Category = "Memory Pairs")
        void InitializeCell(
            int32 NewCellIndex
        );


        UFUNCTION(BlueprintCallable, Category = "Memory Pairs")
        void ConfigureCard(
            int32 NewPairId,
            UTexture2D* NewFrontTexture,
            UTexture2D* NewBackTexture
        );


        UFUNCTION(BlueprintCallable, Category = "Memory Pairs")
        void RevealCard();


        UFUNCTION(BlueprintCallable, Category = "Memory Pairs")
        void HideCard();


        UFUNCTION(BlueprintCallable, Category = "Memory Pairs")
        void SetMatched();


        UFUNCTION(BlueprintCallable, Category = "Memory Pairs")
        void ResetCard();


        UFUNCTION(BlueprintPure, Category = "Memory Pairs")
        bool IsAnimating() const;


    protected:

        UPROPERTY(meta = (BindWidget))
        TObjectPtr<UImage> Image_Card;


        UPROPERTY(meta = (BindWidgetOptional))
        TObjectPtr<UImage> Image_Frame;


        UPROPERTY(meta = (BindWidget))
        TObjectPtr<UButton> Button_Click;


    private:

        UPROPERTY()
        TObjectPtr<UTexture2D> FrontTexture;


        UPROPERTY()
        TObjectPtr<UTexture2D> BackTexture;


        UFUNCTION()
        void HandleButtonClicked();


        void StartFlip(
            bool bNewFaceUp
        );


        bool bAnimatingFlip = false;

        bool bFlipTargetFaceUp = false;

        bool bTextureSwapped = false;

        bool bAnimatingMatchPulse = false;

        float AnimationTime = 0.0f;
    };