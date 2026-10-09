#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

#include "QuickTapCellWidget.h"

#include "QuickTapWidget.generated.h"

class UCanvasPanel;
class UButton;
class UPanelWidget;
class UTextBlock;
class UImage;


// ============================================================
// OUTCOME
// ============================================================

UENUM(BlueprintType)
enum class EQuickTapOutcome : uint8
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
struct FQuickTapRoundResult
{
    GENERATED_BODY()


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    EQuickTapOutcome Outcome =
        EQuickTapOutcome::None;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    bool bWon = false;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    float RoundDuration = 0.0f;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    float ElapsedTime = 0.0f;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    float RemainingTime = 0.0f;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 Moves = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 CorrectMoves = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 WrongMoves = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 MissedTargets = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    float AccuracyPercent = 0.0f;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 RemainingLives = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 MaxLives = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 TargetsCompleted = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    int32 TargetGoal = 0;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    float AverageReactionTime = 0.0f;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    float BestReactionTime = 0.0f;


    UPROPERTY(
        BlueprintReadOnly,
        Category = "Quick Tap"
    )
    float WorstReactionTime = 0.0f;
};


// ============================================================
// EVENT
// ============================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnQuickTapRoundFinished,
    FQuickTapRoundResult,
    Result
);


// ============================================================
// WIDGET
// ============================================================

UCLASS()
class VR_GAMES_HEADLESS_API UQuickTapWidget : public UUserWidget
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
        Category = "Quick Tap|Game",
        meta = (ClampMin = "1")
    )
    int32 TargetGoal = 20;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Game",
        meta = (ClampMin = "2")
    )
    int32 ActiveCellCount = 16;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Game",
        meta = (ClampMin = "1")
    )
    int32 MaxLives = 3;


    // ========================================================
    // DIFFICULTY
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Difficulty",
        meta = (ClampMin = "0.1")
    )
    float StartTargetLifetime = 1.50f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Difficulty",
        meta = (ClampMin = "0.1")
    )
    float MinTargetLifetime = 0.55f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Difficulty",
        meta = (ClampMin = "0.0")
    )
    float TargetLifetimeStep = 0.05f;


    // ========================================================
    // COLORS
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Colors"
    )
    FLinearColor IdleCellColor =
        FLinearColor(
            0.16f,
            0.18f,
            0.21f,
            1.0f
        );


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Colors"
    )
    FLinearColor TargetCellColor =
        FLinearColor(
            0.72f,
            0.25f,
            0.48f,
            1.0f
        );


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Colors"
    )
    FLinearColor WrongCellColor =
        FLinearColor(
            0.90f,
            0.12f,
            0.12f,
            1.0f
        );


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Colors"
    )
    FLinearColor MissCellColor =
        FLinearColor(
            1.0f,
            0.55f,
            0.05f,
            1.0f
        );


    // ========================================================
    // ROUND TIMER
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Timer"
    )
    bool bUseRoundTimer = true;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Timer",
        meta = (ClampMin = "1.0")
    )
    float RoundDurationSeconds = 45.0f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Quick Tap|Timer",
        meta = (ClampMin = "0.01")
    )
    float TimerUpdateInterval = 0.05f;


    // ========================================================
    // STATE
    // ========================================================

    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    int32 RemainingLives = 3;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    int32 Moves = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    int32 CorrectMoves = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    int32 WrongMoves = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    int32 MissedTargets = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    int32 TargetsCompleted = 0;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    int32 CurrentTargetIndex = -1;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    float CurrentTargetLifetime = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    float ElapsedRoundTime = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    float RemainingRoundTime = 0.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    bool bGameFinished = false;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    bool bInputLocked = false;


    UPROPERTY(BlueprintReadOnly, Category = "Quick Tap|State")
    bool bTutorialOpen = false;


    // ========================================================
    // CALLBACK
    // ========================================================

    UPROPERTY(
        BlueprintAssignable,
        Category = "Quick Tap|Events"
    )
    FOnQuickTapRoundFinished OnRoundFinished;


    // ========================================================
    // API
    // ========================================================

    UFUNCTION(
        BlueprintCallable,
        Category = "Quick Tap"
    )
    void RestartGame();


    UFUNCTION(
        BlueprintPure,
        Category = "Quick Tap"
    )
    FQuickTapRoundResult GetCurrentRoundResult() const;


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
    TObjectPtr<UTextBlock> ScoreText;


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

    // ========================================================
    // CELLS
    // ========================================================

    UPROPERTY()
    TArray<TObjectPtr<UQuickTapCellWidget>> Cells;


    // ========================================================
    // TIMERS
    // ========================================================

    FTimerHandle RoundTimerHandle;

    FTimerHandle TargetTimerHandle;


    float RoundStartWorldTime = 0.0f;

    float AccumulatedPausedTime = 0.0f;

    float TutorialPauseStartWorldTime = 0.0f;


    // ========================================================
    // TARGET
    // ========================================================

    int32 PreviousTargetIndex = -1;


    TArray<float> ReactionTimes;


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
        EQuickTapCellAnimationType AnimationType
    );


    // ========================================================
    // BUTTONS
    // ========================================================

    UFUNCTION()
    void HandleRestartClicked();


    UFUNCTION()
    void HandleInfoClicked();


    // ========================================================
    // TIMERS
    // ========================================================

    UFUNCTION()
    void UpdateRoundTimer();


    UFUNCTION()
    void HandleTargetExpired();


    void StartRoundTimer();

    void StopRoundTimer();

    void ClearTargetTimer();

    void RefreshRoundTime();

    void UpdateTargetTimerPauseState();


    // ========================================================
    // GAME
    // ========================================================

    void CollectCells();

    void ConfigureBoardCells();

    void SpawnNextTarget();

    void UpdateHUD();

    void UpdateTimerText();

    void FinishGame(
        EQuickTapOutcome Outcome
    );


    // ========================================================
    // HELPERS
    // ========================================================

    void ConfigureInputWidgets();


    float GetNextTargetLifetime() const;

    float GetCurrentReactionTime() const;


    FString FormatTime(
        float Seconds
    ) const;


    UQuickTapCellWidget* GetCell(
        int32 CellIndex
    ) const;
};