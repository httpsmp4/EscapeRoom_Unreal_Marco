#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

#include "MemoryPairCellWidget.h"

#include "MemoryPairsWidget.generated.h"

class UCanvasPanel;
class UButton;
class UImage;
class UPanelWidget;
class UTextBlock;
class UTexture2D;


UENUM(BlueprintType)
enum class EMemoryPairsOutcome : uint8
{
    None        UMETA(DisplayName = "None"),
    Won         UMETA(DisplayName = "Won"),
    LostLives   UMETA(DisplayName = "Lost Lives"),
    LostTime    UMETA(DisplayName = "Lost Time")
};


USTRUCT(BlueprintType)
struct FMemoryPairsResult
{
    GENERATED_BODY()


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    EMemoryPairsOutcome Outcome =
        EMemoryPairsOutcome::None;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    bool bWon = false;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    float ElapsedTime = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    float RemainingTime = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    int32 Moves = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    int32 CorrectPairs = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    int32 WrongPairs = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    float AccuracyPercent = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    int32 RemainingLives = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    int32 MaxLives = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs")
    int32 TotalPairs = 0;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnMemoryPairsFinished,
    FMemoryPairsResult,
    Result
);


UCLASS()
class VR_GAMES_HEADLESS_API UMemoryPairsWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;


    // ========================================================
    // TEXTURES
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Textures"
    )
    TObjectPtr<UTexture2D> CardBackTexture;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Textures"
    )
    TArray<TObjectPtr<UTexture2D>> PairTextures;


    // ========================================================
    // GAME
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Game",
        meta = (ClampMin = "1")
    )
    int32 PairCount = 8;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Game",
        meta = (ClampMin = "1")
    )
    int32 MaxLives = 6;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Game"
    )
    float WrongPairDisplayTime = 0.70f;


    // ========================================================
    // TIMER
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Timer"
    )
    bool bUseRoundTimer = true;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Timer"
    )
    float RoundDurationSeconds = 90.0f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Memory Pairs|Timer"
    )
    float TimerUpdateInterval = 0.05f;


    // ========================================================
    // STATE
    // ========================================================

    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    int32 RemainingLives = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    int32 Moves = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    int32 CorrectPairs = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    int32 WrongPairs = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    int32 ActivePairCount = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    float ElapsedRoundTime = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    float RemainingRoundTime = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    bool bGameFinished = false;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    bool bInputLocked = false;


    UPROPERTY(BlueprintReadOnly, Category = "Memory Pairs|State")
    bool bTutorialOpen = false;


    // ========================================================
    // EVENT
    // ========================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Memory Pairs|Events"
    )
    FOnMemoryPairsFinished OnRoundFinished;


    UFUNCTION(BlueprintCallable, Category = "Memory Pairs")
    void RestartGame();


    UFUNCTION(BlueprintPure, Category = "Memory Pairs")
    FMemoryPairsResult GetCurrentRoundResult() const;


protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel> BoardRoot;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> Image_Background;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> Image_BoardFrame;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> LevelText;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> MovesText;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> PairsText;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> TimerText;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UPanelWidget> LivesRoot;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UButton> InfoButton;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UButton> RestartButton;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UPanelWidget> TutorialRoot;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UPanelWidget> ResultRoot;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> ResultText;


private:

    UPROPERTY()
    TArray<TObjectPtr<UMemoryPairCellWidget>> Cells;


    int32 FirstSelectedIndex = -1;

    int32 SecondSelectedIndex = -1;


    FTimerHandle RoundTimerHandle;

    FTimerHandle MismatchTimerHandle;


    float RoundStartWorldTime = 0.0f;

    float AccumulatedPausedTime = 0.0f;

    float TutorialPauseStartWorldTime = 0.0f;


    UFUNCTION()
    void HandleCardClicked(
        int32 CellIndex
    );


    UFUNCTION()
    void HandleCardFlipFinished(
        int32 CellIndex,
        bool bIsFaceUp
    );


    UFUNCTION()
    void ResolveMismatch();


    UFUNCTION()
    void HandleRestartClicked();


    UFUNCTION()
    void HandleInfoClicked();


    UFUNCTION()
    void UpdateRoundTimer();


    void CollectCells();

    void SetupCards();

    void ShuffleCards(
        TArray<int32>& CardIds
    );


    void EvaluateSelectedCards();

    void ResetSelection();


    void StartRoundTimer();

    void StopRoundTimer();

    void RefreshRoundTime();


    void ConfigureInputWidgets();

    void UpdateHUD();

    void UpdateTimerText();


    void FinishGame(
        EMemoryPairsOutcome Outcome
    );


    FString FormatTime(
        float Seconds
    ) const;


    UMemoryPairCellWidget* GetCell(
        int32 CellIndex
    ) const;
};