// Fill out your copyright notice in the Description page of Project Settings.
#include "MemoryPairsWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "TimerManager.h"


// ============================================================
// CONSTRUCT
// ============================================================

void UMemoryPairsWidget::NativeConstruct()
{
    Super::NativeConstruct();


    ConfigureInputWidgets();


    if (RestartButton)
    {
        RestartButton->OnClicked.RemoveDynamic(
            this,
            &UMemoryPairsWidget::HandleRestartClicked
        );


        RestartButton->OnClicked.AddDynamic(
            this,
            &UMemoryPairsWidget::HandleRestartClicked
        );
    }


    if (InfoButton)
    {
        InfoButton->OnClicked.RemoveDynamic(
            this,
            &UMemoryPairsWidget::HandleInfoClicked
        );


        InfoButton->OnClicked.AddDynamic(
            this,
            &UMemoryPairsWidget::HandleInfoClicked
        );
    }


    CollectCells();

    RestartGame();
}


// ============================================================
// DESTRUCT
// ============================================================

void UMemoryPairsWidget::NativeDestruct()
{
    StopRoundTimer();


    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(
            MismatchTimerHandle
        );
    }


    Super::NativeDestruct();
}


// ============================================================
// CONFIGURE UI
// ============================================================

void UMemoryPairsWidget::ConfigureInputWidgets()
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


    if (PairsText)
    {
        PairsText->SetVisibility(
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

void UMemoryPairsWidget::CollectCells()
{
    Cells.Empty();


    if (!BoardRoot)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("MemoryPairs: BoardRoot non trovato.")
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


        UMemoryPairCellWidget* Cell =
            Cast<UMemoryPairCellWidget>(
                Child
            );


        if (!Cell)
        {
            continue;
        }


        Cell->InitializeCell(
            RuntimeIndex
        );


        Cell->OnMemoryCardClicked.RemoveDynamic(
            this,
            &UMemoryPairsWidget::HandleCardClicked
        );


        Cell->OnMemoryCardClicked.AddDynamic(
            this,
            &UMemoryPairsWidget::HandleCardClicked
        );


        Cell->OnMemoryCardFlipFinished.RemoveDynamic(
            this,
            &UMemoryPairsWidget::HandleCardFlipFinished
        );


        Cell->OnMemoryCardFlipFinished.AddDynamic(
            this,
            &UMemoryPairsWidget::HandleCardFlipFinished
        );


        Cells.Add(
            Cell
        );


        ++RuntimeIndex;
    }


    UE_LOG(
        LogTemp,
        Log,
        TEXT("MemoryPairs: trovate %d celle."),
        Cells.Num()
    );
}


// ============================================================
// RESTART
// ============================================================

void UMemoryPairsWidget::RestartGame()
{
    StopRoundTimer();


    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(
            MismatchTimerHandle
        );
    }


    RemainingLives =
        MaxLives;


    Moves =
        0;


    CorrectPairs =
        0;


    WrongPairs =
        0;


    ActivePairCount =
        0;


    FirstSelectedIndex =
        -1;


    SecondSelectedIndex =
        -1;


    ElapsedRoundTime =
        0.0f;


    RemainingRoundTime =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;


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


    SetupCards();

    UpdateHUD();

    StartRoundTimer();
}


// ============================================================
// SETUP CARDS
// ============================================================

void UMemoryPairsWidget::SetupCards()
{
    const int32 MaxPairsByCells =
        Cells.Num() / 2;


    const int32 MaxPairsByTextures =
        PairTextures.Num();


    ActivePairCount =
        FMath::Min3(
            PairCount,
            MaxPairsByCells,
            MaxPairsByTextures
        );


    if (ActivePairCount <= 0)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "MemoryPairs: servono almeno 1 texture coppia "
                "e 2 celle."
            )
        );


        for (
            UMemoryPairCellWidget* Cell :
            Cells
            )
        {
            if (Cell)
            {
                Cell->SetVisibility(
                    ESlateVisibility::Collapsed
                );
            }
        }


        return;
    }


    TArray<int32> CardIds;


    for (
        int32 PairIndex = 0;
        PairIndex < ActivePairCount;
        ++PairIndex
        )
    {
        CardIds.Add(
            PairIndex
        );


        CardIds.Add(
            PairIndex
        );
    }


    ShuffleCards(
        CardIds
    );


    const int32 ActiveCardCount =
        ActivePairCount * 2;


    for (
        int32 CellIndex = 0;
        CellIndex < Cells.Num();
        ++CellIndex
        )
    {
        UMemoryPairCellWidget* Cell =
            Cells[CellIndex];


        if (!Cell)
        {
            continue;
        }


        if (
            CellIndex >=
            ActiveCardCount
            )
        {
            Cell->SetVisibility(
                ESlateVisibility::Collapsed
            );


            continue;
        }


        const int32 PairId =
            CardIds[CellIndex];


        Cell->ConfigureCard(
            PairId,
            PairTextures[PairId],
            CardBackTexture
        );
    }
}


// ============================================================
// SHUFFLE
// ============================================================

void UMemoryPairsWidget::ShuffleCards(
    TArray<int32>& CardIds
)
{
    for (
        int32 Index = CardIds.Num() - 1;
        Index > 0;
        --Index
        )
    {
        const int32 RandomIndex =
            FMath::RandRange(
                0,
                Index
            );


        CardIds.Swap(
            Index,
            RandomIndex
        );
    }
}


// ============================================================
// GET CELL
// ============================================================

UMemoryPairCellWidget*
UMemoryPairsWidget::GetCell(
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
// CARD CLICK
// ============================================================

void UMemoryPairsWidget::HandleCardClicked(
    int32 CellIndex
)
{
    if (
        bGameFinished ||
        bInputLocked ||
        bTutorialOpen
        )
    {
        return;
    }


    UMemoryPairCellWidget* Cell =
        GetCell(
            CellIndex
        );


    if (!Cell)
    {
        return;
    }


    if (
        Cell->CardState !=
        EMemoryCardState::Hidden
        )
    {
        return;
    }


    if (FirstSelectedIndex < 0)
    {
        FirstSelectedIndex =
            CellIndex;


        Cell->RevealCard();

        return;
    }


    if (
        CellIndex ==
        FirstSelectedIndex
        )
    {
        return;
    }


    SecondSelectedIndex =
        CellIndex;


    bInputLocked =
        true;


    Cell->RevealCard();
}


// ============================================================
// FLIP FINISHED
// ============================================================

void UMemoryPairsWidget::HandleCardFlipFinished(
    int32 CellIndex,
    bool bIsFaceUp
)
{
    if (bGameFinished)
    {
        return;
    }


    /*
     * Ci interessa valutare solo quando
     * la seconda carta ha finito di girarsi.
     */
    if (
        bIsFaceUp &&
        CellIndex ==
        SecondSelectedIndex
        )
    {
        EvaluateSelectedCards();
    }
}


// ============================================================
// EVALUATE
// ============================================================

void UMemoryPairsWidget::EvaluateSelectedCards()
{
    UMemoryPairCellWidget* FirstCard =
        GetCell(
            FirstSelectedIndex
        );


    UMemoryPairCellWidget* SecondCard =
        GetCell(
            SecondSelectedIndex
        );


    if (
        !FirstCard ||
        !SecondCard
        )
    {
        ResetSelection();

        return;
    }


    ++Moves;


    // ========================================================
    // MATCH
    // ========================================================

    if (
        FirstCard->PairId ==
        SecondCard->PairId
        )
    {
        ++CorrectPairs;


        FirstCard->SetMatched();

        SecondCard->SetMatched();


        ResetSelection();


        if (
            CorrectPairs >=
            ActivePairCount
            )
        {
            FinishGame(
                EMemoryPairsOutcome::Won
            );


            return;
        }


        UpdateHUD();

        return;
    }


    // ========================================================
    // MISMATCH
    // ========================================================

    ++WrongPairs;


    RemainingLives =
        FMath::Max(
            0,
            RemainingLives - 1
        );


    UpdateHUD();


    if (RemainingLives <= 0)
    {
        /*
         * Prima facciamo vedere comunque
         * le due carte sbagliate.
         */
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(
                MismatchTimerHandle,
                this,
                &UMemoryPairsWidget::ResolveMismatch,
                WrongPairDisplayTime,
                false
            );
        }


        return;
    }


    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            MismatchTimerHandle,
            this,
            &UMemoryPairsWidget::ResolveMismatch,
            WrongPairDisplayTime,
            false
        );
    }
}


// ============================================================
// RESOLVE MISMATCH
// ============================================================

void UMemoryPairsWidget::ResolveMismatch()
{
    UMemoryPairCellWidget* FirstCard =
        GetCell(
            FirstSelectedIndex
        );


    UMemoryPairCellWidget* SecondCard =
        GetCell(
            SecondSelectedIndex
        );


    if (FirstCard)
    {
        FirstCard->HideCard();
    }


    if (SecondCard)
    {
        SecondCard->HideCard();
    }


    ResetSelection();


    if (RemainingLives <= 0)
    {
        FinishGame(
            EMemoryPairsOutcome::LostLives
        );
    }
}


// ============================================================
// RESET SELECTION
// ============================================================

void UMemoryPairsWidget::ResetSelection()
{
    FirstSelectedIndex =
        -1;


    SecondSelectedIndex =
        -1;


    bInputLocked =
        false;
}


// ============================================================
// TIMER
// ============================================================

void UMemoryPairsWidget::StartRoundTimer()
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


    World->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &UMemoryPairsWidget::UpdateRoundTimer,
        FMath::Max(
            0.01f,
            TimerUpdateInterval
        ),
        true
    );


    UpdateRoundTimer();
}


void UMemoryPairsWidget::StopRoundTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(
            RoundTimerHandle
        );
    }
}


void UMemoryPairsWidget::RefreshRoundTime()
{
    UWorld* World =
        GetWorld();


    if (!World)
    {
        return;
    }


    const float CurrentWorldTime =
        World->GetTimeSeconds();


    float PausedTime =
        AccumulatedPausedTime;


    if (bTutorialOpen)
    {
        PausedTime +=
            CurrentWorldTime -
            TutorialPauseStartWorldTime;
    }


    ElapsedRoundTime =
        FMath::Max(
            0.0f,
            CurrentWorldTime -
            RoundStartWorldTime -
            PausedTime
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


void UMemoryPairsWidget::UpdateRoundTimer()
{
    if (bGameFinished)
    {
        return;
    }


    RefreshRoundTime();


    if (
        bUseRoundTimer &&
        RemainingRoundTime <= 0.0f
        )
    {
        FinishGame(
            EMemoryPairsOutcome::LostTime
        );


        return;
    }


    UpdateTimerText();
}


// ============================================================
// FINISH
// ============================================================

void UMemoryPairsWidget::FinishGame(
    EMemoryPairsOutcome Outcome
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


    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(
            MismatchTimerHandle
        );
    }


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
        case EMemoryPairsOutcome::Won:
            Message =
                TEXT("COMPLETATO!");
            break;


        case EMemoryPairsOutcome::LostLives:
            Message =
                TEXT("GAME OVER");
            break;


        case EMemoryPairsOutcome::LostTime:
            Message =
                TEXT("TEMPO SCADUTO");
            break;


        default:
            Message =
                TEXT("FINE PARTITA");
            break;
        }


        ResultText->SetText(
            FText::FromString(
                Message
            )
        );
    }


    FMemoryPairsResult Result =
        GetCurrentRoundResult();


    Result.Outcome =
        Outcome;


    Result.bWon =
        Outcome ==
        EMemoryPairsOutcome::Won;


    OnRoundFinished.Broadcast(
        Result
    );
}


// ============================================================
// RESULT
// ============================================================

FMemoryPairsResult
UMemoryPairsWidget::GetCurrentRoundResult() const
{
    FMemoryPairsResult Result;


    Result.ElapsedTime =
        ElapsedRoundTime;


    Result.RemainingTime =
        RemainingRoundTime;


    Result.Moves =
        Moves;


    Result.CorrectPairs =
        CorrectPairs;


    Result.WrongPairs =
        WrongPairs;


    if (Moves > 0)
    {
        Result.AccuracyPercent =
            (
                static_cast<float>(
                    CorrectPairs
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


    Result.TotalPairs =
        ActivePairCount;


    return Result;
}


// ============================================================
// HUD
// ============================================================

void UMemoryPairsWidget::UpdateHUD()
{
    if (MovesText)
    {
        MovesText->SetText(
            FText::AsNumber(
                Moves
            )
        );
    }


    if (PairsText)
    {
        PairsText->SetText(
            FText::Format(
                FText::FromString(
                    TEXT("{0}/{1}")
                ),
                FText::AsNumber(
                    CorrectPairs
                ),
                FText::AsNumber(
                    ActivePairCount
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


void UMemoryPairsWidget::UpdateTimerText()
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
// FORMAT
// ============================================================

FString UMemoryPairsWidget::FormatTime(
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

void UMemoryPairsWidget::HandleRestartClicked()
{
    RestartGame();
}


// ============================================================
// INFO
// ============================================================

void UMemoryPairsWidget::HandleInfoClicked()
{
    if (!TutorialRoot)
    {
        return;
    }


    UWorld* World =
        GetWorld();


    if (!bTutorialOpen)
    {
        bTutorialOpen =
            true;


        if (World)
        {
            TutorialPauseStartWorldTime =
                World->GetTimeSeconds();


            World->GetTimerManager().PauseTimer(
                MismatchTimerHandle
            );
        }


        TutorialRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }
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


            World->GetTimerManager().UnPauseTimer(
                MismatchTimerHandle
            );
        }


        TutorialPauseStartWorldTime =
            0.0f;


        bTutorialOpen =
            false;


        TutorialRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }


    RefreshRoundTime();

    UpdateTimerText();
}