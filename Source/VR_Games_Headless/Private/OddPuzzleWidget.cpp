#include "OddPuzzleWidget.h"

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

void UOddPuzzleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ConfigureInputWidgets();

    if (RestartButton)
    {
        RestartButton->OnClicked.RemoveDynamic(
            this,
            &UOddPuzzleWidget::HandleRestartClicked
        );

        RestartButton->OnClicked.AddDynamic(
            this,
            &UOddPuzzleWidget::HandleRestartClicked
        );
    }

    if (InfoButton)
    {
        InfoButton->OnClicked.RemoveDynamic(
            this,
            &UOddPuzzleWidget::HandleInfoClicked
        );

        InfoButton->OnClicked.AddDynamic(
            this,
            &UOddPuzzleWidget::HandleInfoClicked
        );
    }

    CollectCells();

    RestartGame();
}


// ============================================================
// DESTRUCT
// ============================================================

void UOddPuzzleWidget::NativeDestruct()
{
    StopRoundTimer();

    Super::NativeDestruct();
}


// ============================================================
// UI INPUT
// ============================================================

void UOddPuzzleWidget::ConfigureInputWidgets()
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

    if (RoundText)
    {
        RoundText->SetVisibility(
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
            CanvasSlot->SetZOrder(500);
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
            CanvasSlot->SetZOrder(500);
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

void UOddPuzzleWidget::CollectCells()
{
    Cells.Empty();

    if (!BoardRoot)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "OddPuzzle: BoardRoot non trovato."
            )
        );

        return;
    }


    int32 RuntimeIndex = 0;


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


        UOddPuzzleCellWidget* Cell =
            Cast<UOddPuzzleCellWidget>(
                Child
            );


        if (!Cell)
        {
            continue;
        }


        Cell->InitializeCell(
            RuntimeIndex
        );


        Cell->OnOddCellClicked.RemoveDynamic(
            this,
            &UOddPuzzleWidget::HandleCellClicked
        );

        Cell->OnOddCellClicked.AddDynamic(
            this,
            &UOddPuzzleWidget::HandleCellClicked
        );


        Cell->OnOddCellAnimationFinished.RemoveDynamic(
            this,
            &UOddPuzzleWidget::HandleCellAnimationFinished
        );

        Cell->OnOddCellAnimationFinished.AddDynamic(
            this,
            &UOddPuzzleWidget::HandleCellAnimationFinished
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
            "OddPuzzle: trovate %d celle."
        ),
        Cells.Num()
    );
}


// ============================================================
// RESTART
// ============================================================

void UOddPuzzleWidget::RestartGame()
{
    StopRoundTimer();


    CurrentRound = 1;
    RoundsCompleted = 0;

    RemainingLives = MaxLives;

    Moves = 0;
    CorrectMoves = 0;
    WrongMoves = 0;

    OddCellIndex = -1;
    PreviousOddCellIndex = -1;

    ElapsedRoundTime = 0.0f;

    RemainingRoundTime =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;

    AccumulatedPausedTime = 0.0f;
    TutorialPauseStartWorldTime = 0.0f;

    bGameFinished = false;
    bInputLocked = false;
    bTutorialOpen = false;


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


    SetupCurrentRound();

    UpdateHUD();

    StartRoundTimer();
}


// ============================================================
// SETUP ROUND
// ============================================================

void UOddPuzzleWidget::SetupCurrentRound()
{
    if (Cells.Num() <= 0)
    {
        return;
    }


    const int32 CellsToUse =
        FMath::Clamp(
            ActiveCellCount,
            2,
            Cells.Num()
        );


    // ========================================================
    // CHOOSE ODD CELL
    // ========================================================

    int32 NewOddIndex =
        FMath::RandRange(
            0,
            CellsToUse - 1
        );


    /*
     * Se possibile evitiamo che sia la stessa
     * del round precedente.
     */
    if (
        CellsToUse > 1 &&
        NewOddIndex ==
        PreviousOddCellIndex
        )
    {
        NewOddIndex =
            (NewOddIndex + 1) %
            CellsToUse;
    }


    OddCellIndex =
        NewOddIndex;


    PreviousOddCellIndex =
        OddCellIndex;


    const FLinearColor OddColor =
        GetOddColor();


    // ========================================================
    // CONFIGURE CELLS
    // ========================================================

    for (
        int32 CellIndex = 0;
        CellIndex < Cells.Num();
        ++CellIndex
        )
    {
        UOddPuzzleCellWidget* Cell =
            Cells[CellIndex];


        if (!Cell)
        {
            continue;
        }


        const bool bShouldBeActive =
            CellIndex <
            CellsToUse;


        if (!bShouldBeActive)
        {
            Cell->ConfigureCell(
                BaseCellColor,
                false,
                false
            );

            continue;
        }


        const bool bIsOdd =
            CellIndex ==
            OddCellIndex;


        Cell->ConfigureCell(
            bIsOdd
            ? OddColor
            : BaseCellColor,

            bIsOdd,

            true
        );
    }


    bInputLocked = false;


    UpdateHUD();
}


// ============================================================
// COLOR DIFFERENCE
// ============================================================

float UOddPuzzleWidget::GetCurrentColorDifference() const
{
    if (RoundCount <= 1)
    {
        return StartColorDifference;
    }


    const float Alpha =
        FMath::Clamp(
            static_cast<float>(
                CurrentRound - 1
                ) /
            static_cast<float>(
                RoundCount - 1
                ),
            0.0f,
            1.0f
        );


    return FMath::Lerp(
        StartColorDifference,
        EndColorDifference,
        Alpha
    );
}


// ============================================================
// ODD COLOR
// ============================================================

FLinearColor UOddPuzzleWidget::GetOddColor() const
{
    const float Difference =
        GetCurrentColorDifference();


    /*
     * Se il colore base � gi� molto luminoso,
     * rendiamo l'Odd pi� scuro.
     *
     * Altrimenti lo rendiamo pi� chiaro.
     */

    const float Brightness =
        (
            BaseCellColor.R +
            BaseCellColor.G +
            BaseCellColor.B
            ) /
        3.0f;


    const float Direction =
        Brightness > 0.60f
        ? -1.0f
        : 1.0f;


    return FLinearColor(
        FMath::Clamp(
            BaseCellColor.R +
            Difference * Direction,
            0.0f,
            1.0f
        ),

        FMath::Clamp(
            BaseCellColor.G +
            Difference * Direction,
            0.0f,
            1.0f
        ),

        FMath::Clamp(
            BaseCellColor.B +
            Difference * Direction,
            0.0f,
            1.0f
        ),

        BaseCellColor.A
    );
}


// ============================================================
// GET CELL
// ============================================================

UOddPuzzleCellWidget*
UOddPuzzleWidget::GetCell(
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

void UOddPuzzleWidget::HandleCellClicked(
    int32 CellIndex
)
{
    if (bGameFinished)
    {
        return;
    }


    if (bInputLocked)
    {
        return;
    }


    if (bTutorialOpen)
    {
        return;
    }


    UOddPuzzleCellWidget* Cell =
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

    bInputLocked = true;


    // ========================================================
    // CORRECT
    // ========================================================

    if (
        CellIndex ==
        OddCellIndex
        )
    {
        ++CorrectMoves;

        Cell->PlaySuccessAnimation();
    }

    // ========================================================
    // WRONG
    // ========================================================
    else
    {
        ++WrongMoves;

        RemainingLives =
            FMath::Max(
                0,
                RemainingLives - 1
            );


        Cell->PlayWrongAnimation();
    }


    UpdateHUD();
}


// ============================================================
// ANIMATION FINISHED
// ============================================================

void UOddPuzzleWidget::HandleCellAnimationFinished(
    int32 CellIndex,
    bool bWasSuccess
)
{
    bInputLocked = false;


    // ========================================================
    // CORRECT
    // ========================================================

    if (bWasSuccess)
    {
        ++RoundsCompleted;


        if (
            RoundsCompleted >=
            RoundCount
            )
        {
            FinishGame(
                EOddPuzzleOutcome::Won
            );

            return;
        }


        ++CurrentRound;


        SetupCurrentRound();

        return;
    }


    // ========================================================
    // WRONG
    // ========================================================

    if (RemainingLives <= 0)
    {
        FinishGame(
            EOddPuzzleOutcome::LostLives
        );

        return;
    }


    UpdateHUD();
}


// ============================================================
// TIMER START
// ============================================================

void UOddPuzzleWidget::StartRoundTimer()
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


    AccumulatedPausedTime = 0.0f;
    TutorialPauseStartWorldTime = 0.0f;


    ElapsedRoundTime = 0.0f;


    RemainingRoundTime =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;


    World->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &UOddPuzzleWidget::UpdateRoundTimer,
        FMath::Max(
            0.01f,
            TimerUpdateInterval
        ),
        true
    );


    UpdateRoundTimer();
}


// ============================================================
// TIMER STOP
// ============================================================

void UOddPuzzleWidget::StopRoundTimer()
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
// REFRESH TIME
// ============================================================

void UOddPuzzleWidget::RefreshTimeValues()
{
    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    const float CurrentWorldTime =
        World->GetTimeSeconds();


    float EffectivePausedTime =
        AccumulatedPausedTime;


    if (bTutorialOpen)
    {
        EffectivePausedTime +=
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
            EffectivePausedTime
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
// UPDATE TIMER
// ============================================================

void UOddPuzzleWidget::UpdateRoundTimer()
{
    if (bGameFinished)
    {
        StopRoundTimer();

        return;
    }


    RefreshTimeValues();


    if (
        bUseRoundTimer &&
        RemainingRoundTime <= 0.0f
        )
    {
        RemainingRoundTime = 0.0f;


        UpdateTimerText();


        FinishGame(
            EOddPuzzleOutcome::LostTime
        );


        return;
    }


    UpdateTimerText();
}


// ============================================================
// FINISH
// ============================================================

void UOddPuzzleWidget::FinishGame(
    EOddPuzzleOutcome Outcome
)
{
    if (bGameFinished)
    {
        return;
    }


    RefreshTimeValues();


    bGameFinished = true;
    bInputLocked = true;


    StopRoundTimer();


    UpdateTimerText();


    if (ResultRoot)
    {
        ResultRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (ResultText)
    {
        FString ResultString;


        switch (Outcome)
        {
        case EOddPuzzleOutcome::Won:
            ResultString =
                TEXT("COMPLETATO!");
            break;


        case EOddPuzzleOutcome::LostLives:
            ResultString =
                TEXT("GAME OVER");
            break;


        case EOddPuzzleOutcome::LostTime:
            ResultString =
                TEXT("TEMPO SCADUTO");
            break;


        default:
            ResultString =
                TEXT("FINE ROUND");
            break;
        }


        ResultText->SetText(
            FText::FromString(
                ResultString
            )
        );
    }


    FOddPuzzleRoundResult Result =
        GetCurrentRoundResult();


    Result.Outcome =
        Outcome;


    Result.bWon =
        Outcome ==
        EOddPuzzleOutcome::Won;


    OnRoundFinished.Broadcast(
        Result
    );
}


// ============================================================
// RESULT
// ============================================================

FOddPuzzleRoundResult
UOddPuzzleWidget::GetCurrentRoundResult() const
{
    FOddPuzzleRoundResult Result;


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


    Result.RoundsCompleted =
        RoundsCompleted;


    Result.TotalRounds =
        RoundCount;


    return Result;
}


// ============================================================
// HUD
// ============================================================

void UOddPuzzleWidget::UpdateHUD()
{
    if (MovesText)
    {
        MovesText->SetText(
            FText::AsNumber(
                Moves
            )
        );
    }


    if (RoundText)
    {
        RoundText->SetText(
            FText::Format(
                FText::FromString(
                    TEXT("ROUND {0}/{1}")
                ),

                FText::AsNumber(
                    CurrentRound
                ),

                FText::AsNumber(
                    RoundCount
                )
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

void UOddPuzzleWidget::UpdateTimerText()
{
    if (!TimerText)
    {
        return;
    }


    const float TimeToDisplay =
        bUseRoundTimer
        ? RemainingRoundTime
        : ElapsedRoundTime;


    TimerText->SetText(
        FText::FromString(
            FormatTime(
                TimeToDisplay
            )
        )
    );
}


// ============================================================
// FORMAT
// ============================================================

FString UOddPuzzleWidget::FormatTime(
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
        TotalSeconds / 60;


    const int32 SecondsPart =
        TotalSeconds % 60;


    return FString::Printf(
        TEXT("%02d:%02d"),
        Minutes,
        SecondsPart
    );
}


// ============================================================
// RESTART
// ============================================================

void UOddPuzzleWidget::HandleRestartClicked()
{
    RestartGame();
}


// ============================================================
// INFO
// ============================================================

void UOddPuzzleWidget::HandleInfoClicked()
{
    if (!TutorialRoot)
    {
        return;
    }


    UWorld* World =
        GetWorld();


    if (!bTutorialOpen)
    {
        // APERTURA

        bTutorialOpen = true;


        if (World)
        {
            TutorialPauseStartWorldTime =
                World->GetTimeSeconds();
        }


        TutorialRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }
    else
    {
        // CHIUSURA

        if (World)
        {
            AccumulatedPausedTime +=
                FMath::Max(
                    0.0f,

                    World->GetTimeSeconds() -
                    TutorialPauseStartWorldTime
                );
        }


        bTutorialOpen = false;


        TutorialPauseStartWorldTime =
            0.0f;


        TutorialRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }


    RefreshTimeValues();

    UpdateTimerText();
}