#include "OddPuzzleCellWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"


void UOddPuzzleCellWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetRenderTransformPivot(
        FVector2D(0.5f, 0.5f)
    );

    if (Button_Click)
    {
        Button_Click->OnClicked.RemoveDynamic(
            this,
            &UOddPuzzleCellWidget::HandleButtonClicked
        );

        Button_Click->OnClicked.AddDynamic(
            this,
            &UOddPuzzleCellWidget::HandleButtonClicked
        );
    }

    ResetVisualState();
}


// ============================================================
// INITIALIZE
// ============================================================

void UOddPuzzleCellWidget::InitializeCell(
    int32 NewCellIndex
)
{
    CellIndex = NewCellIndex;

    ResetVisualState();
}


// ============================================================
// CONFIGURE
// ============================================================

void UOddPuzzleCellWidget::ConfigureCell(
    const FLinearColor& NewColor,
    bool bNewOddCell,
    bool bNewActive
)
{
    bIsOddCell = bNewOddCell;

    ResetVisualState();

    SetCellColor(
        NewColor
    );

    SetCellActive(
        bNewActive
    );
}


// ============================================================
// COLOR
// ============================================================

void UOddPuzzleCellWidget::SetCellColor(
    const FLinearColor& NewColor
)
{
    if (Border_Color)
    {
        Border_Color->SetBrushColor(
            NewColor
        );
    }
}


// ============================================================
// ACTIVE
// ============================================================

void UOddPuzzleCellWidget::SetCellActive(
    bool bNewActive
)
{
    bActive = bNewActive;

    if (bActive)
    {
        SetVisibility(
            ESlateVisibility::Visible
        );

        SetRenderOpacity(1.0f);

        if (Button_Click)
        {
            Button_Click->SetIsEnabled(true);
        }
    }
    else
    {
        if (Button_Click)
        {
            Button_Click->SetIsEnabled(false);
        }

        SetVisibility(
            ESlateVisibility::Collapsed
        );
    }
}


// ============================================================
// RESET VISUAL
// ============================================================

void UOddPuzzleCellWidget::ResetVisualState()
{
    bAnimatingSuccess = false;
    bAnimatingWrong = false;

    AnimationTime = 0.0f;

    SetRenderOpacity(1.0f);

    SetRenderTranslation(
        FVector2D::ZeroVector
    );

    SetRenderScale(
        FVector2D(1.0f, 1.0f)
    );

    if (Button_Click)
    {
        Button_Click->SetIsEnabled(
            bActive
        );
    }
}


// ============================================================
// CLICK
// ============================================================

void UOddPuzzleCellWidget::HandleButtonClicked()
{
    if (!bActive)
    {
        return;
    }

    if (IsAnimating())
    {
        return;
    }

    OnOddCellClicked.Broadcast(
        CellIndex
    );
}


// ============================================================
// IS ANIMATING
// ============================================================

bool UOddPuzzleCellWidget::IsAnimating() const
{
    return
        bAnimatingSuccess ||
        bAnimatingWrong;
}


// ============================================================
// SUCCESS
// ============================================================

void UOddPuzzleCellWidget::PlaySuccessAnimation()
{
    if (!bActive || IsAnimating())
    {
        return;
    }

    bAnimatingSuccess = true;
    bAnimatingWrong = false;

    AnimationTime = 0.0f;

    if (Button_Click)
    {
        Button_Click->SetIsEnabled(false);
    }
}


// ============================================================
// WRONG
// ============================================================

void UOddPuzzleCellWidget::PlayWrongAnimation()
{
    if (!bActive || IsAnimating())
    {
        return;
    }

    bAnimatingWrong = true;
    bAnimatingSuccess = false;

    AnimationTime = 0.0f;

    if (Button_Click)
    {
        Button_Click->SetIsEnabled(false);
    }
}


// ============================================================
// TICK
// ============================================================

void UOddPuzzleCellWidget::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime
)
{
    Super::NativeTick(
        MyGeometry,
        InDeltaTime
    );

    // ========================================================
    // SUCCESS
    // ========================================================

    if (bAnimatingSuccess)
    {
        AnimationTime += InDeltaTime;

        const float Duration =
            FMath::Max(
                SuccessAnimationDuration,
                0.01f
            );

        const float Alpha =
            FMath::Clamp(
                AnimationTime / Duration,
                0.0f,
                1.0f
            );

        /*
         * Piccolo POP:
         * 1.0 -> 1.22 -> 1.0
         */
        const float Pulse =
            FMath::Sin(
                Alpha * PI
            );

        const float Scale =
            1.0f +
            Pulse * 0.22f;

        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );

        /*
         * Piccolo flash.
         */
        const float Opacity =
            FMath::Lerp(
                1.0f,
                0.65f,
                Pulse
            );

        SetRenderOpacity(
            Opacity
        );

        if (Alpha >= 1.0f)
        {
            bAnimatingSuccess = false;

            SetRenderScale(
                FVector2D(1.0f, 1.0f)
            );

            SetRenderOpacity(1.0f);

            if (Button_Click)
            {
                Button_Click->SetIsEnabled(
                    bActive
                );
            }

            OnOddCellAnimationFinished.Broadcast(
                CellIndex,
                true
            );
        }

        return;
    }


    // ========================================================
    // WRONG SHAKE
    // ========================================================

    if (bAnimatingWrong)
    {
        AnimationTime += InDeltaTime;

        const float Duration =
            FMath::Max(
                WrongAnimationDuration,
                0.01f
            );

        const float Alpha =
            FMath::Clamp(
                AnimationTime / Duration,
                0.0f,
                1.0f
            );

        const float CurrentStrength =
            ShakeStrength *
            (1.0f - Alpha);

        const float Oscillation =
            FMath::Sin(
                Alpha *
                PI *
                2.0f *
                ShakeFrequency
            );

        SetRenderTranslation(
            FVector2D(
                Oscillation *
                CurrentStrength,
                0.0f
            )
        );

        const float Pulse =
            FMath::Sin(
                Alpha * PI
            );

        const float Scale =
            1.0f -
            Pulse * 0.07f;

        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );

        if (Alpha >= 1.0f)
        {
            bAnimatingWrong = false;

            SetRenderTranslation(
                FVector2D::ZeroVector
            );

            SetRenderScale(
                FVector2D(1.0f, 1.0f)
            );

            if (
                Button_Click &&
                bActive
                )
            {
                Button_Click->SetIsEnabled(true);
            }

            OnOddCellAnimationFinished.Broadcast(
                CellIndex,
                false
            );
        }
    }
}