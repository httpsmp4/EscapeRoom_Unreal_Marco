#include "QuickTapWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

#include "Engine/World.h"
#include "TimerManager.h"


// ============================================================
// CONSTRUCT
// ============================================================

void UQuickTapWidget::NativeConstruct()
{
    Super::NativeConstruct();


    ConfigureInputWidgets();


    if (RestartButton)
    {
        RestartButton->OnClicked.RemoveDynamic(
            this,
            &UQuickTapWidget::HandleRestartClicked
        );


        RestartButton->OnClicked.AddDynamic(
            this,
            &UQuickTapWidget::HandleRestartClicked
        );
    }


    if (InfoButton)
    {
        InfoButton->OnClicked.RemoveDynamic(
            this,
            &UQuickTapWidget::HandleInfoClicked
        );


        InfoButton->OnClicked.AddDynamic(
            this,
            &UQuickTapWidget::HandleInfoClicked
        );
    }


    CollectCells();


    RestartGame();
}


// ============================================================
// DESTRUCT
// ============================================================

void UQuickTapWidget::NativeDestruct()
{
    StopRoundTimer();

    ClearTargetTimer();


    Super::NativeDestruct();
}


// ============================================================
// INPUT UI
// ============================================================

void UQuickTapWidget::ConfigureInputWidgets()
{
    if (Image_Background)
    {
        Image_Background->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (Image_BoardFrame)
    {
        Image_BoardFrame->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (LevelText)
    {
        LevelText->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (ScoreText)
    {
        ScoreText->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (MovesText)
    {
        MovesText->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (TimerText)
    {
        TimerText->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (LivesRoot)
    {
        LivesRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (BoardRoot)
    {
        BoardRoot->SetVisibility(
            ESlateVisibility::SelfHitTestInvisible
        );
    }


    if (InfoButton)
    {
        UCanvasPanelSlot* CanvasSlot =
            Cast<UCanvasPanelSlot>(
                InfoButton->Slot
            );


        if (CanvasSlot)
        {
            CanvasSlot->SetZOrder(
                500
            );
        }
    }


    if (RestartButton)
    {
        UCanvasPanelSlot* CanvasSlot =
            Cast<UCanvasPanelSlot>(
                RestartButton->Slot
            );


        if (CanvasSlot)
        {
            CanvasSlot->SetZOrder(
                500
            );
        }
    }


    if (TutorialRoot)
    {
        TutorialRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }


    if (ResultRoot)
    {
        ResultRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }
}


// ============================================================
// COLLECT CELLS
// ============================================================

void UQuickTapWidget::CollectCells()
{
    Cells.Empty();


    if (!BoardRoot)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "QuickTap: BoardRoot non trovato."
            )
        );


        return;
    }


    int32 RuntimeIndex =
        0;


    for (
        int32 ChildIndex = 0;
        ChildIndex < BoardRoot->GetChildrenCount();
        ++ChildIndex
        )
    {
        UWidget* Child =
            BoardRoot->GetChildAt(
                ChildIndex
            );


        UQuickTapCellWidget* Cell =
            Cast<UQuickTapCellWidget>(
                Child
            );


        if (!Cell)
        {
            continue;
        }


        Cell->InitializeCell(
            RuntimeIndex
        );


        Cell->OnQuickTapCellClicked.RemoveDynamic(
            this,
            &UQuickTapWidget::HandleCellClicked
        );


        Cell->OnQuickTapCellClicked.AddDynamic(
            this,
            &UQuickTapWidget::HandleCellClicked
        );


        Cell->OnQuickTapCellAnimationFinished.RemoveDynamic(
            this,
            &UQuickTapWidget::HandleCellAnimationFinished
        );


        Cell->OnQuickTapCellAnimationFinished.AddDynamic(
            this,
            &UQuickTapWidget::HandleCellAnimationFinished
        );


        Cells.Add(
            Cell
        );


        ++RuntimeIndex;
    }


    UE_LOG(
        LogTemp,
        Log,
        TEXT(
            "QuickTap: trovate %d celle."
        ),
        Cells.Num()
    );
}


// ============================================================
// RESTART
// ============================================================

void UQuickTapWidget::RestartGame()
{
    StopRoundTimer();

    ClearTargetTimer();


    RemainingLives =
        MaxLives;


    Moves =
        0;


    CorrectMoves =
        0;


    WrongMoves =
        0;


    MissedTargets =
        0;


    TargetsCompleted =
        0;


    CurrentTargetIndex =
        -1;


    PreviousTargetIndex =
        -1;


    CurrentTargetLifetime =
        StartTargetLifetime;


    ReactionTimes.Empty();


    ElapsedRoundTime =
        0.0f;


    RemainingRoundTime =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;


    RoundStartWorldTime =
        0.0f;


    AccumulatedPausedTime =
        0.0f;


    TutorialPauseStartWorldTime =
        0.0f;


    bGameFinished =
        false;


    bInputLocked =
        false;


    bTutorialOpen =
        false;


    if (TutorialRoot)
    {
        TutorialRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }


    if (ResultRoot)
    {
        ResultRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }


    ConfigureBoardCells();


    UpdateHUD();


    StartRoundTimer();


    SpawnNextTarget();
}


// ============================================================
// BOARD
// ============================================================

void UQuickTapWidget::ConfigureBoardCells()
{
    const int32 CellsToUse =
        FMath::Clamp(
            ActiveCellCount,
            0,
            Cells.Num()
        );


    for (
        int32 CellIndex = 0;
        CellIndex < Cells.Num();
        ++CellIndex
        )
    {
        UQuickTapCellWidget* Cell =
            Cells[CellIndex];


        if (!Cell)
        {
            continue;
        }


        const bool bShouldBeActive =
            CellIndex <
            CellsToUse;


        Cell->ConfigureCell(
            IdleCellColor,
            false,
            bShouldBeActive
        );
    }
}


// ============================================================
// SPAWN TARGET
// ============================================================

void UQuickTapWidget::SpawnNextTarget()
{
    if (bGameFinished)
    {
        return;
    }


    const int32 CellsToUse =
        FMath::Clamp(
            ActiveCellCount,
            0,
            Cells.Num()
        );


    if (CellsToUse <= 0)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "QuickTap: nessuna cella disponibile."
            )
        );


        return;
    }


    // --------------------------------------------------------
    // RESET ACTIVE CELLS
    // --------------------------------------------------------

    for (
        int32 CellIndex = 0;
        CellIndex < CellsToUse;
        ++CellIndex
        )
    {
        UQuickTapCellWidget* Cell =
            Cells[CellIndex];


        if (!Cell)
        {
            continue;
        }


        Cell->ConfigureCell(
            IdleCellColor,
            false,
            true
        );
    }


    // --------------------------------------------------------
    // RANDOM TARGET
    // --------------------------------------------------------

    int32 NewTarget =
        FMath::RandRange(
            0,
            CellsToUse - 1
        );


    if (
        CellsToUse > 1 &&
        NewTarget ==
        PreviousTargetIndex
        )
    {
        NewTarget =
            (
                NewTarget + 1
                ) %
            CellsToUse;
    }


    CurrentTargetIndex =
        NewTarget;


    PreviousTargetIndex =
        NewTarget;


    CurrentTargetLifetime =
        GetNextTargetLifetime();


    UQuickTapCellWidget* TargetCell =
        GetCell(
            CurrentTargetIndex
        );


    if (TargetCell)
    {
        TargetCell->ConfigureCell(
            TargetCellColor,
            true,
            true
        );
    }


    // --------------------------------------------------------
    // TARGET TIMER
    // --------------------------------------------------------

    UWorld* World =
        GetWorld();


    if (World)
    {
        World->GetTimerManager().SetTimer(
            TargetTimerHandle,
            this,
            &UQuickTapWidget::HandleTargetExpired,
            CurrentTargetLifetime,
            false
        );
    }


    bInputLocked =
        false;


    UpdateTargetTimerPauseState();


    UpdateHUD();
}


// ============================================================
// TARGET LIFETIME
// ============================================================

float UQuickTapWidget::GetNextTargetLifetime() const
{
    const float NewLifetime =
        StartTargetLifetime -
        (
            static_cast<float>(
                TargetsCompleted
                ) *
            TargetLifetimeStep
            );


    return FMath::Clamp(
        NewLifetime,
        MinTargetLifetime,
        StartTargetLifetime
    );
}


// ============================================================
// GET CELL
// ============================================================

UQuickTapCellWidget*
UQuickTapWidget::GetCell(
    int32 CellIndex
) const
{
    if (!Cells.IsValidIndex(CellIndex))
    {
        return nullptr;
    }


    return Cells[CellIndex];
}


// ============================================================
// CLICK
// ============================================================

void UQuickTapWidget::HandleCellClicked(
    int32 CellIndex
)
{
    if (bGameFinished)
    {
        return;
    }


    if (bTutorialOpen)
    {
        return;
    }


    if (bInputLocked)
    {
        return;
    }


    UQuickTapCellWidget* Cell =
        GetCell(
            CellIndex
        );


    if (!Cell)
    {
        return;
    }


    if (!Cell->bActive)
    {
        return;
    }


    if (Cell->IsAnimating())
    {
        return;
    }


    ++Moves;


    // ========================================================
    // TARGET CORRETTO
    // ========================================================

    if (
        CellIndex ==
        CurrentTargetIndex
        )
    {
        const float ReactionTime =
            GetCurrentReactionTime();


        if (ReactionTime >= 0.0f)
        {
            ReactionTimes.Add(
                ReactionTime
            );
        }


        ++CorrectMoves;

        ++TargetsCompleted;


        bInputLocked =
            true;


        ClearTargetTimer();


        Cell->PlaySuccessAnimation();


        UpdateHUD();


        return;
    }


    // ========================================================
    // TARGET SBAGLIATO
    // ========================================================

    ++WrongMoves;


    RemainingLives =
        FMath::Max(
            0,
            RemainingLives - 1
        );


    bInputLocked =
        true;


    Cell->SetCellColor(
        WrongCellColor
    );


    UpdateTargetTimerPauseState();


    Cell->PlayWrongAnimation();


    UpdateHUD();
}


// ============================================================
// TARGET EXPIRED
// ============================================================

void UQuickTapWidget::HandleTargetExpired()
{
    if (bGameFinished)
    {
        return;
    }


    if (bTutorialOpen)
    {
        return;
    }


    ++MissedTargets;


    RemainingLives =
        FMath::Max(
            0,
            RemainingLives - 1
        );


    bInputLocked =
        true;


    UQuickTapCellWidget* TargetCell =
        GetCell(
            CurrentTargetIndex
        );


    if (TargetCell)
    {
        TargetCell->SetCellColor(
            MissCellColor
        );


        TargetCell->PlayMissAnimation();
    }
    else
    {
        bInputLocked =
            false;


        if (RemainingLives <= 0)
        {
            FinishGame(
                EQuickTapOutcome::LostLives
            );
        }
        else
        {
            SpawnNextTarget();
        }
    }


    UpdateHUD();
}


// ============================================================
// ANIMATION FINISHED
// ============================================================

void UQuickTapWidget::HandleCellAnimationFinished(
    int32 CellIndex,
    EQuickTapCellAnimationType AnimationType
)
{
    if (bGameFinished)
    {
        return;
    }


    UQuickTapCellWidget* Cell =
        GetCell(
            CellIndex
        );


    // ========================================================
    // SUCCESS
    // ========================================================

    if (
        AnimationType ==
        EQuickTapCellAnimationType::Success
        )
    {
        bInputLocked =
            false;


        if (
            TargetsCompleted >=
            TargetGoal
            )
        {
            FinishGame(
                EQuickTapOutcome::Won
            );


            return;
        }


        SpawnNextTarget();


        return;
    }


    // ========================================================
    // WRONG
    // ========================================================

    if (
        AnimationType ==
        EQuickTapCellAnimationType::Wrong
        )
    {
        /*
         * Ripristiniamo la cella sbagliata.
         */
        if (Cell)
        {
            Cell->SetCellColor(
                IdleCellColor
            );
        }


        bInputLocked =
            false;


        if (RemainingLives <= 0)
        {
            FinishGame(
                EQuickTapOutcome::LostLives
            );


            return;
        }


        /*
         * Il target corretto rimane quello attuale.
         */
        UpdateTargetTimerPauseState();


        UpdateHUD();


        return;
    }


    // ========================================================
    // MISS
    // ========================================================

    if (
        AnimationType ==
        EQuickTapCellAnimationType::Miss
        )
    {
        bInputLocked =
            false;


        if (RemainingLives <= 0)
        {
            FinishGame(
                EQuickTapOutcome::LostLives
            );


            return;
        }


        SpawnNextTarget();
    }
}


// ============================================================
// REACTION TIME
// ============================================================

float UQuickTapWidget::GetCurrentReactionTime() const
{
    UWorld* World =
        GetWorld();


    if (!World)
    {
        return -1.0f;
    }


    const float Remaining =
        World->GetTimerManager().GetTimerRemaining(
            TargetTimerHandle
        );


    if (Remaining < 0.0f)
    {
        return -1.0f;
    }


    return FMath::Clamp(
        CurrentTargetLifetime -
        Remaining,
        0.0f,
        CurrentTargetLifetime
    );
}


// ============================================================
// TARGET TIMER PAUSE STATE
// ============================================================

void UQuickTapWidget::UpdateTargetTimerPauseState()
{
    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    /*
     * Il timer target si ferma:
     * - durante il tutorial
     * - durante shake/error animation
     */
    const bool bShouldPause =
        bTutorialOpen ||
        bInputLocked;


    if (bShouldPause)
    {
        World->GetTimerManager().PauseTimer(
            TargetTimerHandle
        );
    }
    else
    {
        World->GetTimerManager().UnPauseTimer(
            TargetTimerHandle
        );
    }
}


// ============================================================
// CLEAR TARGET TIMER
// ============================================================

void UQuickTapWidget::ClearTargetTimer()
{
    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    World->GetTimerManager().ClearTimer(
        TargetTimerHandle
    );
}


// ============================================================
// START ROUND TIMER
// ============================================================

void UQuickTapWidget::StartRoundTimer()
{
    StopRoundTimer();


    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    RoundStartWorldTime =
        World->GetTimeSeconds();


    AccumulatedPausedTime =
        0.0f;


    TutorialPauseStartWorldTime =
        0.0f;


    ElapsedRoundTime =
        0.0f;


    RemainingRoundTime =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;


    World->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &UQuickTapWidget::UpdateRoundTimer,
        FMath::Max(
            0.01f,
            TimerUpdateInterval
        ),
        true
    );


    UpdateRoundTimer();
}


// ============================================================
// STOP ROUND TIMER
// ============================================================

void UQuickTapWidget::StopRoundTimer()
{
    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    World->GetTimerManager().ClearTimer(
        RoundTimerHandle
    );
}


// ============================================================
// REFRESH ROUND TIME
// ============================================================

void UQuickTapWidget::RefreshRoundTime()
{
    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    const float CurrentWorldTime =
        World->GetTimeSeconds();


    float CurrentPausedTime =
        AccumulatedPausedTime;


    if (bTutorialOpen)
    {
        CurrentPausedTime +=
            FMath::Max(
                0.0f,
                CurrentWorldTime -
                TutorialPauseStartWorldTime
            );
    }


    ElapsedRoundTime =
        FMath::Max(
            0.0f,
            CurrentWorldTime -
            RoundStartWorldTime -
            CurrentPausedTime
        );


    if (bUseRoundTimer)
    {
        RemainingRoundTime =
            FMath::Max(
                0.0f,
                RoundDurationSeconds -
                ElapsedRoundTime
            );
    }
}


// ============================================================
// UPDATE ROUND TIMER
// ============================================================

void UQuickTapWidget::UpdateRoundTimer()
{
    if (bGameFinished)
    {
        StopRoundTimer();

        return;
    }


    RefreshRoundTime();


    if (
        bUseRoundTimer &&
        RemainingRoundTime <= 0.0f
        )
    {
        RemainingRoundTime =
            0.0f;


        UpdateTimerText();


        FinishGame(
            EQuickTapOutcome::LostTime
        );


        return;
    }


    UpdateTimerText();
}


// ============================================================
// FINISH
// ============================================================

void UQuickTapWidget::FinishGame(
    EQuickTapOutcome Outcome
)
{
    if (bGameFinished)
    {
        return;
    }


    RefreshRoundTime();


    bGameFinished =
        true;


    bInputLocked =
        true;


    StopRoundTimer();

    ClearTargetTimer();


    UpdateTimerText();


    if (ResultRoot)
    {
        ResultRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (ResultText)
    {
        FString Message;


        switch (Outcome)
        {
        case EQuickTapOutcome::Won:
        {
            Message =
                TEXT("COMPLETATO!");

            break;
        }


        case EQuickTapOutcome::LostLives:
        {
            Message =
                TEXT("GAME OVER");

            break;
        }


        case EQuickTapOutcome::LostTime:
        {
            Message =
                TEXT("TEMPO SCADUTO");

            break;
        }


        default:
        {
            Message =
                TEXT("FINE ROUND");

            break;
        }
        }


        ResultText->SetText(
            FText::FromString(
                Message
            )
        );
    }


    FQuickTapRoundResult Result =
        GetCurrentRoundResult();


    Result.Outcome =
        Outcome;


    Result.bWon =
        Outcome ==
        EQuickTapOutcome::Won;


    OnRoundFinished.Broadcast(
        Result
    );
}


// ============================================================
// RESULT
// ============================================================

FQuickTapRoundResult
UQuickTapWidget::GetCurrentRoundResult() const
{
    FQuickTapRoundResult Result;


    Result.RoundDuration =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;


    Result.ElapsedTime =
        ElapsedRoundTime;


    Result.RemainingTime =
        RemainingRoundTime;


    Result.Moves =
        Moves;


    Result.CorrectMoves =
        CorrectMoves;


    Result.WrongMoves =
        WrongMoves;


    Result.MissedTargets =
        MissedTargets;


    if (Moves > 0)
    {
        Result.AccuracyPercent =
            (
                static_cast<float>(
                    CorrectMoves
                    ) /
                static_cast<float>(
                    Moves
                    )
                ) *
            100.0f;
    }


    Result.RemainingLives =
        RemainingLives;


    Result.MaxLives =
        MaxLives;


    Result.TargetsCompleted =
        TargetsCompleted;


    Result.TargetGoal =
        TargetGoal;


    // --------------------------------------------------------
    // REACTION STATS
    // --------------------------------------------------------

    if (ReactionTimes.Num() > 0)
    {
        float Total =
            0.0f;


        float Best =
            ReactionTimes[0];


        float Worst =
            ReactionTimes[0];


        for (
            float ReactionTime :
        ReactionTimes
            )
        {
            Total +=
                ReactionTime;


            Best =
                FMath::Min(
                    Best,
                    ReactionTime
                );


            Worst =
                FMath::Max(
                    Worst,
                    ReactionTime
                );
        }


        Result.AverageReactionTime =
            Total /
            static_cast<float>(
                ReactionTimes.Num()
                );


        Result.BestReactionTime =
            Best;


        Result.WorstReactionTime =
            Worst;
    }


    return Result;
}


// ============================================================
// HUD
// ============================================================

void UQuickTapWidget::UpdateHUD()
{
    if (ScoreText)
    {
        ScoreText->SetText(
            FText::Format(
                FText::FromString(
                    TEXT("SCORE {0}/{1}")
                ),

                FText::AsNumber(
                    TargetsCompleted
                ),

                FText::AsNumber(
                    TargetGoal
                )
            )
        );
    }


    if (MovesText)
    {
        MovesText->SetText(
            FText::AsNumber(
                Moves
            )
        );
    }


    if (LivesRoot)
    {
        for (
            int32 LifeIndex = 0;
            LifeIndex <
            LivesRoot->GetChildrenCount();
            ++LifeIndex
            )
        {
            UWidget* LifeWidget =
                LivesRoot->GetChildAt(
                    LifeIndex
                );


            if (!LifeWidget)
            {
                continue;
            }


            LifeWidget->SetVisibility(
                LifeIndex < RemainingLives
                ? ESlateVisibility::HitTestInvisible
                : ESlateVisibility::Hidden
            );
        }
    }


    UpdateTimerText();
}


// ============================================================
// TIMER TEXT
// ============================================================

void UQuickTapWidget::UpdateTimerText()
{
    if (!TimerText)
    {
        return;
    }


    const float DisplayTime =
        bUseRoundTimer
        ? RemainingRoundTime
        : ElapsedRoundTime;


    TimerText->SetText(
        FText::FromString(
            FormatTime(
                DisplayTime
            )
        )
    );
}


// ============================================================
// FORMAT TIME
// ============================================================

FString UQuickTapWidget::FormatTime(
    float Seconds
) const
{
    const int32 TotalSeconds =
        FMath::Max(
            0,
            FMath::CeilToInt(
                Seconds
            )
        );


    const int32 Minutes =
        TotalSeconds /
        60;


    const int32 SecondsPart =
        TotalSeconds %
        60;


    return FString::Printf(
        TEXT("%02d:%02d"),
        Minutes,
        SecondsPart
    );
}


// ============================================================
// RESTART
// ============================================================

void UQuickTapWidget::HandleRestartClicked()
{
    RestartGame();
}


// ============================================================
// INFO
// ============================================================

void UQuickTapWidget::HandleInfoClicked()
{
    if (!TutorialRoot)
    {
        return;
    }


    UWorld* World =
        GetWorld();


    // ========================================================
    // OPEN
    // ========================================================

    if (!bTutorialOpen)
    {
        bTutorialOpen =
            true;


        if (World)
        {
            TutorialPauseStartWorldTime =
                World->GetTimeSeconds();
        }


        TutorialRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );


        UpdateTargetTimerPauseState();
    }

    // ========================================================
    // CLOSE
    // ========================================================

    else
    {
        if (World)
        {
            AccumulatedPausedTime +=
                FMath::Max(
                    0.0f,
                    World->GetTimeSeconds() -
                    TutorialPauseStartWorldTime
                );
        }


        TutorialPauseStartWorldTime =
            0.0f;


        bTutorialOpen =
            false;


        TutorialRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );


        UpdateTargetTimerPauseState();
    }


    RefreshRoundTime();

    UpdateTimerText();
}