#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

#include "ArrowPuzzleCellWidget.h"

#include "ArrowPuzzleWidget.generated.h"

class UCanvasPanel;
class UButton;
class UPanelWidget;
class UTextBlock;
class UImage;


// ============================================================
// OUTCOME
// ============================================================

UENUM(BlueprintType)
enum class EArrowPuzzleRoundOutcome : uint8
{
    None        UMETA(DisplayName = "None"),
    Won         UMETA(DisplayName = "Won"),
    LostLives   UMETA(DisplayName = "Lost - No Lives"),
    LostTime    UMETA(DisplayName = "Lost - Time Out"),
    Deadlock    UMETA(DisplayName = "Lost - Deadlock")
};


// ============================================================
// RESULT STRUCT
// ============================================================

USTRUCT(BlueprintType)
struct FArrowPuzzleRoundResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    EArrowPuzzleRoundOutcome Outcome =
        EArrowPuzzleRoundOutcome::None;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    bool bWon = false;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    float RoundDuration = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    float ElapsedTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    float RemainingTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    int32 Moves = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    int32 CorrectMoves = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    int32 WrongMoves = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    float AccuracyPercent = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    int32 RemainingLives = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    int32 MaxLives = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    int32 RemainingArrows = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle")
    int32 TotalArrows = 0;
};


// ============================================================
// DELEGATES
// ============================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnArrowPuzzleFinished,
    bool,
    bWon
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnArrowPuzzleRoundFinished,
    FArrowPuzzleRoundResult,
    Result
);


// ============================================================
// WIDGET
// ============================================================

UCLASS()
class VR_GAMES_HEADLESS_API UArrowPuzzleWidget : public UUserWidget
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
        Category = "Arrow Puzzle|Game"
    )
    int32 GridRows = 6;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Game"
    )
    int32 GridColumns = 6;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Game"
    )
    int32 MaxLives = 3;


    // ========================================================
    // TIMER CONFIG
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Timer"
    )
    bool bUseRoundTimer = true;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Timer",
        meta = (ClampMin = "1.0")
    )
    float RoundDurationSeconds = 60.0f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Arrow Puzzle|Timer",
        meta = (ClampMin = "0.01")
    )
    float TimerUpdateInterval = 0.05f;


    // ========================================================
    // STATE
    // ========================================================

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    int32 RemainingLives = 3;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    int32 Moves = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    int32 CorrectMoves = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    int32 WrongMoves = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    int32 ActiveArrows = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    int32 TotalArrows = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    float ElapsedRoundTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    float RemainingRoundTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    bool bGameFinished = false;

    UPROPERTY(BlueprintReadOnly, Category = "Arrow Puzzle|State")
    bool bInputLocked = false;


    // ========================================================
    // EVENTS
    // ========================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Arrow Puzzle|Events"
    )
    FOnArrowPuzzleFinished OnGameFinished;

    UPROPERTY(
        BlueprintAssignable,
        Category = "Arrow Puzzle|Events"
    )
    FOnArrowPuzzleRoundFinished OnRoundFinished;


    // ========================================================
    // PUBLIC API
    // ========================================================

    UFUNCTION(BlueprintCallable, Category = "Arrow Puzzle")
    void RestartGame();

    UFUNCTION(BlueprintPure, Category = "Arrow Puzzle")
    bool CanRemoveCell(int32 CellIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Arrow Puzzle")
    void UpdateAvailability();

    UFUNCTION(BlueprintPure, Category = "Arrow Puzzle")
    FArrowPuzzleRoundResult GetCurrentRoundResult() const;


protected:

    // ========================================================
    // REQUIRED
    // ========================================================

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel> BoardRoot;


    // ========================================================
    // OPTIONAL VISUALS
    // ========================================================

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
    TArray<TObjectPtr<UArrowPuzzleCellWidget>> Cells;


    // ========================================================
    // TIMER
    // ========================================================

    FTimerHandle RoundTimerHandle;

    float RoundStartWorldTime = 0.0f;


    // ========================================================
    // CELL CALLBACKS
    // ========================================================

    UFUNCTION()
    void HandleCellClicked(int32 CellIndex);

    UFUNCTION()
    void HandleCellAnimationFinished(
        int32 CellIndex,
        bool bWasRemoval
    );


    // ========================================================
    // BUTTON CALLBACKS
    // ========================================================

    UFUNCTION()
    void HandleRestartClicked();

    UFUNCTION()
    void HandleInfoClicked();


    // ========================================================
    // TIMER CALLBACK
    // ========================================================

    UFUNCTION()
    void UpdateRoundTimer();


    // ========================================================
    // INTERNAL
    // ========================================================

    void StartRoundTimer();
    void StopRoundTimer();

    void CollectCells();

    void RemoveArrow(int32 CellIndex);
    void LoseLife(int32 CellIndex);

    void CheckGameState();

    void FinishGame(
        EArrowPuzzleRoundOutcome Outcome
    );

    void UpdateHUD();
    void UpdateTimerText();

    bool ValidateBoard() const;
    bool HasAnyValidMove() const;

    UArrowPuzzleCellWidget* GetCell(
        int32 CellIndex
    ) const;

    void ConfigureInputWidgets();

    FString FormatTime(
        float Seconds
    ) const;
};