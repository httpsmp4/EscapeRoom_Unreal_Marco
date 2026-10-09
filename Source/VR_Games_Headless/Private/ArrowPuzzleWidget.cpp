// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowPuzzleWidget.h"

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

void UArrowPuzzleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ConfigureInputWidgets();


    // ========================================================
    // RESTART
    // ========================================================

    if (RestartButton)
    {
        RestartButton->OnClicked.RemoveDynamic(
            this,
            &UArrowPuzzleWidget::HandleRestartClicked
        );

        RestartButton->OnClicked.AddDynamic(
            this,
            &UArrowPuzzleWidget::HandleRestartClicked
        );

        RestartButton->SetIsEnabled(true);

        RestartButton->SetVisibility(
            ESlateVisibility::Visible
        );
    }


    // ========================================================
    // INFO
    // ========================================================

    if (InfoButton)
    {
        InfoButton->OnClicked.RemoveDynamic(
            this,
            &UArrowPuzzleWidget::HandleInfoClicked
        );

        InfoButton->OnClicked.AddDynamic(
            this,
            &UArrowPuzzleWidget::HandleInfoClicked
        );

        InfoButton->SetIsEnabled(true);

        InfoButton->SetVisibility(
            ESlateVisibility::Visible
        );
    }


    CollectCells();

    RestartGame();
}


// ============================================================
// DESTRUCT
// ============================================================

void UArrowPuzzleWidget::NativeDestruct()
{
    StopRoundTimer();

    Super::NativeDestruct();
}


// ============================================================
// CONFIGURE INPUT
// ============================================================

void UArrowPuzzleWidget::ConfigureInputWidgets()
{
    /*
     * Elementi puramente grafici:
     * devono lasciar passare completamente il mouse.
     */

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

    if (LivesRoot)
    {
        LivesRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    /*
     * BoardRoot non deve catturare il mouse,
     * ma i WBP_ArrowCell contenuti al suo interno sì.
     */
    if (BoardRoot)
    {
        BoardRoot->SetVisibility(
            ESlateVisibility::SelfHitTestInvisible
        );
    }


    /*
     * IMPORTANTISSIMO:
     *
     * NON usare una variabile chiamata "Slot".
     * UWidget possiede già un membro Slot.
     */

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


    /*
     * Tutorial e Result inizialmente chiusi.
     */

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

void UArrowPuzzleWidget::CollectCells()
{
    Cells.Empty();

    if (!BoardRoot)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "ArrowPuzzle: BoardRoot non trovato."
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

        UArrowPuzzleCellWidget* Cell =
            Cast<UArrowPuzzleCellWidget>(
                Child
            );

        if (!Cell)
        {
            continue;
        }


        Cell->InitializeCell(
            RuntimeIndex
        );


        // ----------------------------------------------------
        // CLICK
        // ----------------------------------------------------

        Cell->OnArrowCellClicked.RemoveDynamic(
            this,
            &UArrowPuzzleWidget::HandleCellClicked
        );

        Cell->OnArrowCellClicked.AddDynamic(
            this,
            &UArrowPuzzleWidget::HandleCellClicked
        );


        // ----------------------------------------------------
        // ANIMATION CALLBACK
        // ----------------------------------------------------

        Cell->OnCellAnimationFinished.RemoveDynamic(
            this,
            &UArrowPuzzleWidget::HandleCellAnimationFinished
        );

        Cell->OnCellAnimationFinished.AddDynamic(
            this,
            &UArrowPuzzleWidget::HandleCellAnimationFinished
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
            "ArrowPuzzle: trovate %d celle."
        ),
        Cells.Num()
    );


    ValidateBoard();
}


// ============================================================
// RESTART
// ============================================================

void UArrowPuzzleWidget::RestartGame()
{
    StopRoundTimer();


    RemainingLives = MaxLives;

    Moves = 0;
    CorrectMoves = 0;
    WrongMoves = 0;

    ActiveArrows = 0;
    TotalArrows = 0;

    ElapsedRoundTime = 0.0f;

    RemainingRoundTime =
        bUseRoundTimer
        ? RoundDurationSeconds
        : 0.0f;

    bGameFinished = false;
    bInputLocked = false;


    // ========================================================
    // RESET CELLS
    // ========================================================

    for (
        UArrowPuzzleCellWidget* Cell :
        Cells
        )
    {
        if (!Cell)
        {
            continue;
        }

        Cell->ResetCell();

        if (Cell->bActive)
        {
            ++ActiveArrows;
            ++TotalArrows;
        }
    }


    // ========================================================
    // HIDE OVERLAYS
    // ========================================================

    if (ResultRoot)
    {
        ResultRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }

    if (TutorialRoot)
    {
        TutorialRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }


    // ========================================================
    // BUTTONS
    // ========================================================

    if (InfoButton)
    {
        InfoButton->SetIsEnabled(true);

        InfoButton->SetVisibility(
            ESlateVisibility::Visible
        );
    }

    if (RestartButton)
    {
        RestartButton->SetIsEnabled(true);

        RestartButton->SetVisibility(
            ESlateVisibility::Visible
        );
    }


    ValidateBoard();

    UpdateAvailability();

    UpdateHUD();


    if (
        ActiveArrows > 0 &&
        !HasAnyValidMove()
        )
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT(
                "ArrowPuzzle: ATTENZIONE - il livello parte in deadlock."
            )
        );
    }


    StartRoundTimer();
}


// ============================================================
// GET CELL
// ============================================================

UArrowPuzzleCellWidget*
UArrowPuzzleWidget::GetCell(
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

void UArrowPuzzleWidget::HandleCellClicked(
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


    UArrowPuzzleCellWidget* Cell =
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


    if (
        CanRemoveCell(
            CellIndex
        )
        )
    {
        ++CorrectMoves;

        RemoveArrow(
            CellIndex
        );
    }
    else
    {
        ++WrongMoves;

        LoseLife(
            CellIndex
        );
    }


    UpdateHUD();
}


// ============================================================
// CELL ANIMATION FINISHED
// ============================================================

void UArrowPuzzleWidget::HandleCellAnimationFinished(
    int32 CellIndex,
    bool bWasRemoval
)
{
    bInputLocked = false;

    UpdateAvailability();

    UpdateHUD();

    CheckGameState();
}


// ============================================================
// CAN REMOVE?
// ============================================================

bool UArrowPuzzleWidget::CanRemoveCell(
    int32 CellIndex
) const
{
    UArrowPuzzleCellWidget* Source =
        GetCell(
            CellIndex
        );

    if (!Source)
    {
        return false;
    }

    if (!Source->bActive)
    {
        return false;
    }


    for (
        UArrowPuzzleCellWidget* Other :
        Cells
        )
    {
        if (!Other)
        {
            continue;
        }

        if (Other == Source)
        {
            continue;
        }

        if (!Other->bActive)
        {
            continue;
        }


        const int32 RowDelta =
            Other->GridRow -
            Source->GridRow;

        const int32 ColumnDelta =
            Other->GridColumn -
            Source->GridColumn;


        switch (
            Source->StartDirection
            )
        {
            // UP
        case EArrowDirection::Up:
        {
            if (
                ColumnDelta == 0 &&
                RowDelta < 0
                )
            {
                return false;
            }

            break;
        }


        // RIGHT
        case EArrowDirection::Right:
        {
            if (
                RowDelta == 0 &&
                ColumnDelta > 0
                )
            {
                return false;
            }

            break;
        }


        // DOWN
        case EArrowDirection::Down:
        {
            if (
                ColumnDelta == 0 &&
                RowDelta > 0
                )
            {
                return false;
            }

            break;
        }


        // LEFT
        case EArrowDirection::Left:
        {
            if (
                RowDelta == 0 &&
                ColumnDelta < 0
                )
            {
                return false;
            }

            break;
        }
        }
    }


    return true;
}


// ============================================================
// REMOVE ARROW
// ============================================================

void UArrowPuzzleWidget::RemoveArrow(
    int32 CellIndex
)
{
    UArrowPuzzleCellWidget* Cell =
        GetCell(
            CellIndex
        );

    if (!Cell)
    {
        bInputLocked = false;
        return;
    }

    if (!Cell->bActive)
    {
        bInputLocked = false;
        return;
    }


    Cell->BeginRemoveAnimation();


    ActiveArrows =
        FMath::Max(
            0,
            ActiveArrows - 1
        );


    /*
     * La freccia è già bActive = false,
     * quindi le altre possono immediatamente
     * diventare disponibili.
     */
    UpdateAvailability();
}


// ============================================================
// WRONG MOVE
// ============================================================

void UArrowPuzzleWidget::LoseLife(
    int32 CellIndex
)
{
    UArrowPuzzleCellWidget* Cell =
        GetCell(
            CellIndex
        );

    if (!Cell)
    {
        bInputLocked = false;
        return;
    }


    Cell->BeginBlockedAnimation();


    RemainingLives =
        FMath::Max(
            0,
            RemainingLives - 1
        );
}


// ============================================================
// AVAILABILITY
// ============================================================

void UArrowPuzzleWidget::UpdateAvailability()
{
    if (bGameFinished)
    {
        return;
    }


    for (
        int32 CellIndex = 0;
        CellIndex < Cells.Num();
        ++CellIndex
        )
    {
        UArrowPuzzleCellWidget* Cell =
            Cells[CellIndex];

        if (!Cell)
        {
            continue;
        }

        if (!Cell->bActive)
        {
            continue;
        }


        Cell->SetAvailable(
            CanRemoveCell(
                CellIndex
            )
        );
    }
}


// ============================================================
// CHECK GAME
// ============================================================

void UArrowPuzzleWidget::CheckGameState()
{
    if (bGameFinished)
    {
        return;
    }


    // ========================================================
    // WIN
    // ========================================================

    if (ActiveArrows <= 0)
    {
        FinishGame(
            EArrowPuzzleRoundOutcome::Won
        );

        return;
    }


    // ========================================================
    // NO LIVES
    // ========================================================

    if (RemainingLives <= 0)
    {
        FinishGame(
            EArrowPuzzleRoundOutcome::LostLives
        );

        return;
    }


    // ========================================================
    // DEADLOCK
    // ========================================================

    if (!HasAnyValidMove())
    {
        FinishGame(
            EArrowPuzzleRoundOutcome::Deadlock
        );

        return;
    }
}


// ============================================================
// START TIMER
// ============================================================

void UArrowPuzzleWidget::StartRoundTimer()
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


    if (bUseRoundTimer)
    {
        RemainingRoundTime =
            FMath::Max(
                0.0f,
                RoundDurationSeconds
            );
    }
    else
    {
        RemainingRoundTime = 0.0f;
    }


    /*
     * Lo facciamo partire anche senza countdown:
     * così ElapsedRoundTime viene comunque calcolato.
     */

    World->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &UArrowPuzzleWidget::UpdateRoundTimer,
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

void UArrowPuzzleWidget::StopRoundTimer()
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

void UArrowPuzzleWidget::UpdateRoundTimer()
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
                EArrowPuzzleRoundOutcome::LostTime
            );

            return;
        }
    }


    UpdateTimerText();
}


// ============================================================
// FINISH GAME
// ============================================================

void UArrowPuzzleWidget::FinishGame(
    EArrowPuzzleRoundOutcome Outcome
)
{
    if (bGameFinished)
    {
        return;
    }


    /*
     * Prima aggiorniamo il tempo finale.
     */

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


    // ========================================================
    // RESULT UI
    // ========================================================

    if (ResultRoot)
    {
        /*
         * VISIBILE ma non intercetta il mouse.
         *
         * Così RestartButton continua a funzionare.
         */
        ResultRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (ResultText)
    {
        FString Message;


        switch (Outcome)
        {
        case EArrowPuzzleRoundOutcome::Won:
            Message = TEXT("COMPLETATO!");
            break;


        case EArrowPuzzleRoundOutcome::LostLives:
            Message = TEXT("GAME OVER");
            break;


        case EArrowPuzzleRoundOutcome::LostTime:
            Message = TEXT("TEMPO SCADUTO");
            break;


        case EArrowPuzzleRoundOutcome::Deadlock:
            Message = TEXT("NESSUNA MOSSA");
            break;


        default:
            Message = TEXT("FINE ROUND");
            break;
        }


        ResultText->SetText(
            FText::FromString(
                Message
            )
        );
    }


    // ========================================================
    // CALLBACK
    // ========================================================

    const bool bWon =
        Outcome ==
        EArrowPuzzleRoundOutcome::Won;


    OnGameFinished.Broadcast(
        bWon
    );


    FArrowPuzzleRoundResult Result =
        GetCurrentRoundResult();


    Result.Outcome = Outcome;
    Result.bWon = bWon;


    OnRoundFinished.Broadcast(
        Result
    );
}


// ============================================================
// GET RESULT
// ============================================================

FArrowPuzzleRoundResult
UArrowPuzzleWidget::GetCurrentRoundResult() const
{
    FArrowPuzzleRoundResult Result;


    Result.Outcome =
        EArrowPuzzleRoundOutcome::None;


    Result.bWon = false;


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
    else
    {
        Result.AccuracyPercent = 0.0f;
    }


    Result.RemainingLives =
        RemainingLives;


    Result.MaxLives =
        MaxLives;


    Result.RemainingArrows =
        ActiveArrows;


    Result.TotalArrows =
        TotalArrows;


    return Result;
}


// ============================================================
// HUD
// ============================================================

void UArrowPuzzleWidget::UpdateHUD()
{
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
        const int32 LifeCount =
            LivesRoot->GetChildrenCount();


        for (
            int32 LifeIndex = 0;
            LifeIndex < LifeCount;
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

void UArrowPuzzleWidget::UpdateTimerText()
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
// FORMAT TIME
// ============================================================

FString UArrowPuzzleWidget::FormatTime(
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
// RESTART BUTTON
// ============================================================

void UArrowPuzzleWidget::HandleRestartClicked()
{
    UE_LOG(
        LogTemp,
        Log,
        TEXT(
            "ArrowPuzzle: RESTART CLICKED"
        )
    );


    RestartGame();
}


// ============================================================
// INFO BUTTON
// ============================================================

void UArrowPuzzleWidget::HandleInfoClicked()
{
    UE_LOG(
        LogTemp,
        Log,
        TEXT(
            "ArrowPuzzle: INFO CLICKED"
        )
    );


    if (!TutorialRoot)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT(
                "ArrowPuzzle: TutorialRoot non trovato."
            )
        );

        return;
    }


    const bool bIsOpen =
        TutorialRoot->GetVisibility() !=
        ESlateVisibility::Collapsed;


    if (bIsOpen)
    {
        TutorialRoot->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }
    else
    {
        /*
         * VISIBILE ma non intercetta i click.
         *
         * In questo modo InfoButton può essere
         * premuto nuovamente per chiuderlo.
         */
        TutorialRoot->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }
}


// ============================================================
// VALIDATE BOARD
// ============================================================

bool UArrowPuzzleWidget::ValidateBoard() const
{
    bool bValid = true;


    TSet<int32> OccupiedCoordinates;


    for (
        UArrowPuzzleCellWidget* Cell :
        Cells
        )
    {
        if (!Cell)
        {
            continue;
        }


        // ====================================================
        // BOUNDS
        // ====================================================

        if (
            Cell->GridRow < 0 ||
            Cell->GridRow >= GridRows ||
            Cell->GridColumn < 0 ||
            Cell->GridColumn >= GridColumns
            )
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "ArrowPuzzle: cella %d fuori griglia. Row=%d Column=%d"
                ),
                Cell->CellIndex,
                Cell->GridRow,
                Cell->GridColumn
            );


            bValid = false;

            continue;
        }


        // ====================================================
        // UNIQUE POSITION
        // ====================================================

        const int32 CoordinateKey =
            Cell->GridRow *
            GridColumns +
            Cell->GridColumn;


        if (
            OccupiedCoordinates.Contains(
                CoordinateKey
            )
            )
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "ArrowPuzzle: DUE CELLE in Row=%d Column=%d"
                ),
                Cell->GridRow,
                Cell->GridColumn
            );


            bValid = false;
        }
        else
        {
            OccupiedCoordinates.Add(
                CoordinateKey
            );
        }
    }


    if (bValid)
    {
        UE_LOG(
            LogTemp,
            Log,
            TEXT(
                "ArrowPuzzle: griglia valida. Celle=%d"
            ),
            Cells.Num()
        );
    }


    return bValid;
}


// ============================================================
// VALID MOVE EXISTS?
// ============================================================

bool UArrowPuzzleWidget::HasAnyValidMove() const
{
    for (
        int32 CellIndex = 0;
        CellIndex < Cells.Num();
        ++CellIndex
        )
    {
        UArrowPuzzleCellWidget* Cell =
            Cells[CellIndex];


        if (!Cell)
        {
            continue;
        }


        if (!Cell->bActive)
        {
            continue;
        }


        if (
            CanRemoveCell(
                CellIndex
            )
            )
        {
            return true;
        }
    }


    return false;
}