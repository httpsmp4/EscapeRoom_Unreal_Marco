
#include "NumberPuzzleWidget.h"

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

void UNumberPuzzleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ConfigureInputWidgets();


    if (RestartButton)
    {
        RestartButton->OnClicked.RemoveDynamic(
            this,
            &UNumberPuzzleWidget::HandleRestartClicked
        );

        RestartButton->OnClicked.AddDynamic(
            this,
            &UNumberPuzzleWidget::HandleRestartClicked
        );
    }


    if (InfoButton)
    {
        InfoButton->OnClicked.RemoveDynamic(
            this,
            &UNumberPuzzleWidget::HandleInfoClicked
        );

        InfoButton->OnClicked.AddDynamic(
            this,
            &UNumberPuzzleWidget::HandleInfoClicked
        );
    }


    CollectCells();

    RestartGame();
}


// ============================================================
// DESTRUCT
// ============================================================

void UNumberPuzzleWidget::NativeDestruct()
{
    StopRoundTimer();

    Super::NativeDestruct();
}


// ============================================================
// CONFIGURE INPUT
// ============================================================

void UNumberPuzzleWidget::ConfigureInputWidgets()
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


    if (TargetText)
    {
        TargetText->SetVisibility(
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

void UNumberPuzzleWidget::CollectCells()
{
    Cells.Empty();


    if (!BoardRoot)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "NumberPuzzle: BoardRoot non trovato."
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


        UNumberPuzzleCellWidget* Cell =
            Cast<UNumberPuzzleCellWidget>(
                Child
            );


        if (!Cell)
        {
            continue;
        }


        Cell->InitializeCell(
            RuntimeIndex
        );


        Cell->OnNumberCellClicked.RemoveDynamic(
            this,
            &UNumberPuzzleWidget::HandleCellClicked
        );

        Cell->OnNumberCellClicked.AddDynamic(
            this,
            &UNumberPuzzleWidget::HandleCellClicked
        );


        Cell->OnNumberCellAnimationFinished.RemoveDynamic(
            this,
            &UNumberPuzzleWidget::HandleCellAnimationFinished
        );

        Cell->OnNumberCellAnimationFinished.AddDynamic(
            this,
            &UNumberPuzzleWidget::HandleCellAnimationFinished
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
            "NumberPuzzle: trovate %d celle."
        ),
        Cells.Num()
    );
}


// ============================================================
// RESTART
// ============================================================

void UNumberPuzzleWidget::RestartGame()
{
    StopRoundTimer();


    CurrentTarget = 1;

    RemainingLives = MaxLives;

    Moves = 0;
    CorrectMoves = 0;
    WrongMoves = 0;

    ElapsedRoundTime = 0.0f;

    RemainingRoundTime =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;

    bGameFinished = false;
    bInputLocked = false;


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


    SetupNumbers();

    UpdateHUD();

    StartRoundTimer();
}


// ============================================================
// SETUP NUMBERS
// ============================================================

void UNumberPuzzleWidget::SetupNumbers()
{
    /*
     * Se hai 36 celle nel Canvas ma NumberCount = 15,
     * vengono usate solo le prime 15.
     */

    TotalNumbers =
        FMath::Min(
            NumberCount,
            Cells.Num()
        );


    if (TotalNumbers <= 0)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "NumberPuzzle: nessuna cella disponibile."
            )
        );

        return;
    }


    TArray<int32> Numbers;


    for (
        int32 Number = 1;
        Number <= TotalNumbers;
        ++Number
        )
    {
        Numbers.Add(
            Number
        );
    }


    ShuffleNumbers(
        Numbers
    );


    for (
        int32 CellIndex = 0;
        CellIndex < Cells.Num();
        ++CellIndex
        )
    {
        UNumberPuzzleCellWidget* Cell =
            Cells[CellIndex];


        if (!Cell)
        {
            continue;
        }


        Cell->ResetVisualState();


        if (CellIndex < TotalNumbers)
        {
            Cell->SetNumber(
                Numbers[CellIndex]
            );

            Cell->SetCellActive(
                true
            );
        }
        else
        {
            Cell->SetCellActive(
                false
            );
        }
    }
}


// ============================================================
// SHUFFLE
// ============================================================

void UNumberPuzzleWidget::ShuffleNumbers(
    TArray<int32>& Numbers
)
{
    /*
     * Fisher-Yates.
     */

    for (
        int32 Index = Numbers.Num() - 1;
        Index > 0;
        --Index
        )
    {
        const int32 SwapIndex =
            FMath::RandRange(
                0,
                Index
            );


        Numbers.Swap(
            Index,
            SwapIndex
        );
    }
}


// ============================================================
// GET CELL
// ============================================================

UNumberPuzzleCellWidget*
UNumberPuzzleWidget::GetCell(
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
// CELL CLICK
// ============================================================

void UNumberPuzzleWidget::HandleCellClicked(
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


    UNumberPuzzleCellWidget* Cell =
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


    ++Moves;

    bInputLocked = true;


    // ========================================================
    // CORRECT
    // ========================================================

    if (
        Cell->NumberValue ==
        CurrentTarget
        )
    {
        ++CorrectMoves;

        Cell->PlaySuccessAnimation();

        ++CurrentTarget;
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

void UNumberPuzzleWidget::HandleCellAnimationFinished(
    int32 CellIndex,
    bool bWasSuccess
)
{
    bInputLocked = false;


    // ========================================================
    // SUCCESS?
    // ========================================================

    if (bWasSuccess)
    {
        /*
         * Esempio con 15:
         *
         * dopo aver cliccato 15,
         * CurrentTarget diventa 16.
         */
        if (
            CurrentTarget >
            TotalNumbers
            )
        {
            FinishGame(
                ENumberPuzzleOutcome::Won
            );

            return;
        }
    }


    // ========================================================
    // LIVES?
    // ========================================================

    if (RemainingLives <= 0)
    {
        FinishGame(
            ENumberPuzzleOutcome::LostLives
        );

        return;
    }


    UpdateHUD();
}


// ============================================================
// START TIMER
// ============================================================

void UNumberPuzzleWidget::StartRoundTimer()
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


    ElapsedRoundTime = 0.0f;


    World->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &UNumberPuzzleWidget::UpdateRoundTimer,
        FMath::Max(
            0.01f,
            TimerUpdateInterval
        ),
        true
    );


    UpdateRoundTimer();
}


// ============================================================
// STOP TIMER
// ============================================================

void UNumberPuzzleWidget::StopRoundTimer()
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
// UPDATE TIMER
// ============================================================

void UNumberPuzzleWidget::UpdateRoundTimer()
{
    if (bGameFinished)
    {
        StopRoundTimer();

        return;
    }


    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    ElapsedRoundTime =
        FMath::Max(
            0.0f,
            World->GetTimeSeconds() -
            RoundStartWorldTime
        );


    if (bUseRoundTimer)
    {
        RemainingRoundTime =
            FMath::Max(
                0.0f,
                RoundDurationSeconds -
                ElapsedRoundTime
            );


        if (RemainingRoundTime <= 0.0f)
        {
            RemainingRoundTime = 0.0f;

            UpdateTimerText();


            FinishGame(
                ENumberPuzzleOutcome::LostTime
            );

            return;
        }
    }


    UpdateTimerText();
}


// ============================================================
// FINISH
// ============================================================

void UNumberPuzzleWidget::FinishGame(
    ENumberPuzzleOutcome Outcome
)
{
    if (bGameFinished)
    {
        return;
    }


    UWorld* World =
        GetWorld();


    if (World)
    {
        ElapsedRoundTime =
            FMath::Max(
                0.0f,
                World->GetTimeSeconds() -
                RoundStartWorldTime
            );
    }


    if (bUseRoundTimer)
    {
        ElapsedRoundTime =
            FMath::Min(
                ElapsedRoundTime,
                RoundDurationSeconds
            );


        RemainingRoundTime =
            FMath::Max(
                0.0f,
                RoundDurationSeconds -
                ElapsedRoundTime
            );
    }


    bGameFinished = true;

    bInputLocked = true;


    StopRoundTimer();

    UpdateTimerText();


    if (ResultRoot)
    {
        /*
         * Non blocca Info / Restart.
         */
        ResultRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (ResultText)
    {
        FString ResultString;


        switch (Outcome)
        {
        case ENumberPuzzleOutcome::Won:
            ResultString = TEXT("COMPLETATO!");
            break;

        case ENumberPuzzleOutcome::LostLives:
            ResultString = TEXT("GAME OVER");
            break;

        case ENumberPuzzleOutcome::LostTime:
            ResultString = TEXT("TEMPO SCADUTO");
            break;

        default:
            ResultString = TEXT("FINE ROUND");
            break;
        }


        ResultText->SetText(
            FText::FromString(
                ResultString
            )
        );
    }


    FNumberPuzzleRoundResult Result =
        GetCurrentRoundResult();


    Result.Outcome = Outcome;

    Result.bWon =
        Outcome ==
        ENumberPuzzleOutcome::Won;


    OnRoundFinished.Broadcast(
        Result
    );
}


// ============================================================
// RESULT
// ============================================================

FNumberPuzzleRoundResult
UNumberPuzzleWidget::GetCurrentRoundResult() const
{
    FNumberPuzzleRoundResult Result;


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


    Result.NumbersCompleted =
        CorrectMoves;


    Result.TotalNumbers =
        TotalNumbers;


    return Result;
}


// ============================================================
// HUD
// ============================================================

void UNumberPuzzleWidget::UpdateHUD()
{
    if (MovesText)
    {
        MovesText->SetText(
            FText::AsNumber(
                Moves
            )
        );
    }


    if (TargetText)
    {
        if (
            CurrentTarget <=
            TotalNumbers
            )
        {
            TargetText->SetText(
                FText::Format(
                    FText::FromString(
                        TEXT("TROVA: {0}")
                    ),
                    FText::AsNumber(
                        CurrentTarget
                    )
                )
            );
        }
        else
        {
            TargetText->SetText(
                FText::GetEmpty()
            );
        }
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

void UNumberPuzzleWidget::UpdateTimerText()
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

FString UNumberPuzzleWidget::FormatTime(
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

void UNumberPuzzleWidget::HandleRestartClicked()
{
    RestartGame();
}


// ============================================================
// INFO
// ============================================================

void UNumberPuzzleWidget::HandleInfoClicked()
{
    if (!TutorialRoot)
    {
        return;
    }


    const bool bOpen =
        TutorialRoot->GetVisibility() !=
        ESlateVisibility::Collapsed;


    TutorialRoot->SetVisibility(
        bOpen
        ? ESlateVisibility::Collapsed
        : ESlateVisibility::HitTestInvisible
    );
}