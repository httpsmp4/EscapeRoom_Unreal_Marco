#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

#include "NumberPuzzleCellWidget.h"

#include "NumberPuzzleWidget.generated.h"

class UCanvasPanel;
class UButton;
class UPanelWidget;
class UTextBlock;
class UImage;


// ============================================================
// OUTCOME
// ============================================================

UENUM(BlueprintType)
enum class ENumberPuzzleOutcome : uint8
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
struct FNumberPuzzleRoundResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    ENumberPuzzleOutcome Outcome =
        ENumberPuzzleOutcome::None;

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
    int32 NumbersCompleted = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalNumbers = 0;
};


// ============================================================
// EVENT
// ============================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnNumberPuzzleFinished,
    FNumberPuzzleRoundResult,
    Result
);


// ============================================================
// WIDGET
// ============================================================

UCLASS()
class VR_GAMES_HEADLESS_API UNumberPuzzleWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;


    // ========================================================
    // GAME CONFIG
    // ========================================================

    /*
     * Quanti numeri usare.
     *
     * Se nel BoardRoot hai 20 celle e imposti 15,
     * le ultime 5 vengono nascoste.
     */
    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Game",
        meta = (ClampMin = "2")
    )
    int32 NumberCount = 15;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Game"
    )
    int32 MaxLives = 3;


    // ========================================================
    // TIMER
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Timer"
    )
    bool bUseRoundTimer = true;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Timer",
        meta = (ClampMin = "1.0")
    )
    float RoundDurationSeconds = 60.0f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Number Puzzle|Timer"
    )
    float TimerUpdateInterval = 0.05f;


    // ========================================================
    // STATE
    // ========================================================

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentTarget = 1;

    UPROPERTY(BlueprintReadOnly)
    int32 RemainingLives = 3;

    UPROPERTY(BlueprintReadOnly)
    int32 Moves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CorrectMoves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 WrongMoves = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalNumbers = 0;

    UPROPERTY(BlueprintReadOnly)
    float ElapsedRoundTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float RemainingRoundTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    bool bGameFinished = false;

    UPROPERTY(BlueprintReadOnly)
    bool bInputLocked = false;


    // ========================================================
    // EVENTS
    // ========================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Number Puzzle|Events"
    )
    FOnNumberPuzzleFinished OnRoundFinished;


    // ========================================================
    // API
    // ========================================================

    UFUNCTION(
        BlueprintCallable,
        Category = "Number Puzzle"
    )
    void RestartGame();


    UFUNCTION(
        BlueprintPure,
        Category = "Number Puzzle"
    )
    FNumberPuzzleRoundResult GetCurrentRoundResult() const;


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
    TObjectPtr<UTextBlock> MovesText;


    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> TimerText;


    /*
     * Ti consiglio di aggiungerlo.
     * Mostrer�:
     *
     * TROVA: 1
     * TROVA: 2
     * ecc.
     */
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> TargetText;


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
    TArray<TObjectPtr<UNumberPuzzleCellWidget>> Cells;


    FTimerHandle RoundTimerHandle;

    float RoundStartWorldTime = 0.0f;


    // ========================================================
    // CELL EVENTS
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


    // ========================================================
    // INTERNAL
    // ========================================================

    void CollectCells();

    void SetupNumbers();

    void ShuffleNumbers(
        TArray<int32>& Numbers
    );

    void UpdateHUD();

    void UpdateTimerText();

    void FinishGame(
        ENumberPuzzleOutcome Outcome
    );

    void ConfigureInputWidgets();

    FString FormatTime(
        float Seconds
    ) const;


    UNumberPuzzleCellWidget* GetCell(
        int32 CellIndex
    ) const;
};