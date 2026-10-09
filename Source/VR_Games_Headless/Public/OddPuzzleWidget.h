#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

#include "OddPuzzleCellWidget.h"

#include "OddPuzzleWidget.generated.h"

class UCanvasPanel;
class UButton;
class UPanelWidget;
class UTextBlock;
class UImage;


// ============================================================
// OUTCOME
// ============================================================

UENUM(BlueprintType)
enum class EOddPuzzleOutcome : uint8
{
    None        UMETA(DisplayName = "None"),
    Won         UMETA(DisplayName = "Won"),
    LostLives   UMETA(DisplayName = "Lost - No Lives"),
    LostTime    UMETA(DisplayName = "Lost - Time Out")
};


// ============================================================
// RESULT
// ============================================================

USTRUCT(BlueprintType)
struct FOddPuzzleRoundResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EOddPuzzleOutcome Outcome =
        EOddPuzzleOutcome::None;

    UPROPERTY(BlueprintReadOnly)
    bool bWon = false;

    UPROPERTY(BlueprintReadOnly)
    float RoundDuration = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float ElapsedTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float RemainingTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 Moves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CorrectMoves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 WrongMoves = 0;

    UPROPERTY(BlueprintReadOnly)
    float AccuracyPercent = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 RemainingLives = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 MaxLives = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 RoundsCompleted = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalRounds = 0;
};


// ============================================================
// DELEGATE
// ============================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnOddPuzzleRoundFinished,
    FOddPuzzleRoundResult,
    Result
);


// ============================================================
// MAIN WIDGET
// ============================================================

UCLASS()
class VR_GAMES_HEADLESS_API UOddPuzzleWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;


    // ========================================================
    // GAME CONFIG
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Game",
        meta = (ClampMin = "1")
    )
    int32 RoundCount = 10;

    /*
     * Quante celle utilizzare.
     *
     * Con il tuo layout 4x4:
     * 16.
     */
    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Game",
        meta = (ClampMin = "2")
    )
    int32 ActiveCellCount = 16;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Game"
    )
    int32 MaxLives = 3;


    // ========================================================
    // COLOR
    // ========================================================

    /*
     * Colore di tutte le celle normali.
     *
     * Modificabile dai Class Defaults.
     */
    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Colors"
    )
    FLinearColor BaseCellColor =
        FLinearColor(
            0.18f,
            0.55f,
            0.90f,
            1.0f
        );

    /*
     * Nei primi round la differenza � evidente.
     */
    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Colors",
        meta = (ClampMin = "0.01", ClampMax = "1.0")
    )
    float StartColorDifference = 0.28f;

    /*
     * Negli ultimi round diventa pi� difficile.
     */
    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Colors",
        meta = (ClampMin = "0.01", ClampMax = "1.0")
    )
    float EndColorDifference = 0.08f;


    // ========================================================
    // TIMER
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Timer"
    )
    bool bUseRoundTimer = true;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Timer",
        meta = (ClampMin = "1.0")
    )
    float RoundDurationSeconds = 60.0f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Odd Puzzle|Timer",
        meta = (ClampMin = "0.01")
    )
    float TimerUpdateInterval = 0.05f;


    // ========================================================
    // STATE
    // ========================================================

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentRound = 1;

    UPROPERTY(BlueprintReadOnly)
    int32 RoundsCompleted = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 RemainingLives = 3;

    UPROPERTY(BlueprintReadOnly)
    int32 Moves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CorrectMoves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 WrongMoves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 OddCellIndex = -1;

    UPROPERTY(BlueprintReadOnly)
    float ElapsedRoundTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float RemainingRoundTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    bool bGameFinished = false;

    UPROPERTY(BlueprintReadOnly)
    bool bInputLocked = false;

    UPROPERTY(BlueprintReadOnly)
    bool bTutorialOpen = false;


    // ========================================================
    // EVENT
    // ========================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Odd Puzzle|Events"
    )
    FOnOddPuzzleRoundFinished OnRoundFinished;


    // ========================================================
    // API
    // ========================================================

    UFUNCTION(BlueprintCallable, Category = "Odd Puzzle")
    void RestartGame();

    UFUNCTION(BlueprintPure, Category = "Odd Puzzle")
    FOddPuzzleRoundResult GetCurrentRoundResult() const;


protected:

    // ========================================================
    // UI
    // ========================================================

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel> BoardRoot;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> Image_Background;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> Image_BoardFrame;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> LevelText;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> RoundText;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> MovesText;

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
    TArray<TObjectPtr<UOddPuzzleCellWidget>> Cells;

    FTimerHandle RoundTimerHandle;

    float RoundStartWorldTime = 0.0f;

    float AccumulatedPausedTime = 0.0f;

    float TutorialPauseStartWorldTime = 0.0f;

    int32 PreviousOddCellIndex = -1;


    // ========================================================
    // CELL
    // ========================================================

    UFUNCTION()
    void HandleCellClicked(
        int32 CellIndex
    );

    UFUNCTION()
    void HandleCellAnimationFinished(
        int32 CellIndex,
        bool bWasSuccess
    );


    // ========================================================
    // BUTTONS
    // ========================================================

    UFUNCTION()
    void HandleRestartClicked();

    UFUNCTION()
    void HandleInfoClicked();


    // ========================================================
    // TIMER
    // ========================================================

    UFUNCTION()
    void UpdateRoundTimer();

    void StartRoundTimer();

    void StopRoundTimer();

    void RefreshTimeValues();


    // ========================================================
    // GAME
    // ========================================================

    void CollectCells();

    void SetupCurrentRound();

    void UpdateHUD();

    void UpdateTimerText();

    void FinishGame(
        EOddPuzzleOutcome Outcome
    );

    void ConfigureInputWidgets();

    FLinearColor GetOddColor() const;

    float GetCurrentColorDifference() const;

    FString FormatTime(
        float Seconds
    ) const;

    UOddPuzzleCellWidget* GetCell(
        int32 CellIndex
    ) const;
};