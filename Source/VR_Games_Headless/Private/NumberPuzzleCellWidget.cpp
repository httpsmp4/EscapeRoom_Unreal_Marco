#include "NumberPuzzleCellWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UNumberPuzzleCellWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetRenderTransformPivot(
        FVector2D(0.5f, 0.5f)
    );

    if (Button_Click)
    {
        Button_Click->OnClicked.RemoveDynamic(
            this,
            &UNumberPuzzleCellWidget::HandleButtonClicked
        );

        Button_Click->OnClicked.AddDynamic(
            this,
            &UNumberPuzzleCellWidget::HandleButtonClicked
        );
    }

    ResetVisualState();
}


// ============================================================
// INITIALIZE
// ============================================================

void UNumberPuzzleCellWidget::InitializeCell(
    int32 NewCellIndex
)
{
    CellIndex = NewCellIndex;

    ResetVisualState();
}


// ============================================================
// NUMBER
// ============================================================

void UNumberPuzzleCellWidget::SetNumber(
    int32 NewNumber
)
{
    NumberValue = NewNumber;

    if (NumberText)
    {
        NumberText->SetText(
            FText::AsNumber(
                NumberValue
            )
        );
    }
}


// ============================================================
// ACTIVE
// ============================================================

void UNumberPuzzleCellWidget::SetCellActive(
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

void UNumberPuzzleCellWidget::ResetVisualState()
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

void UNumberPuzzleCellWidget::HandleButtonClicked()
{
    if (!bActive)
    {
        return;
    }

    if (IsAnimating())
    {
        return;
    }

    OnNumberCellClicked.Broadcast(
        CellIndex
    );
}


// ============================================================
// IS ANIMATING
// ============================================================

bool UNumberPuzzleCellWidget::IsAnimating() const
{
    return
        bAnimatingSuccess ||
        bAnimatingWrong;
}


// ============================================================
// SUCCESS
// ============================================================

void UNumberPuzzleCellWidget::PlaySuccessAnimation()
{
    if (!bActive || IsAnimating())
    {
        return;
    }

    /*
     * Da questo momento non � pi� cliccabile,
     * ma resta visibile fino alla fine del fade.
     */
    bActive = false;

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

void UNumberPuzzleCellWidget::PlayWrongAnimation()
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

void UNumberPuzzleCellWidget::NativeTick(
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
                0.01f,
                SuccessAnimationDuration
            );

        const float Alpha =
            FMath::Clamp(
                AnimationTime / Duration,
                0.0f,
                1.0f
            );

        /*
         * Prima cresce leggermente,
         * poi si riduce.
         */

        float Scale = 1.0f;

        if (Alpha < 0.35f)
        {
            const float LocalAlpha =
                Alpha / 0.35f;

            Scale =
                FMath::Lerp(
                    1.0f,
                    1.15f,
                    LocalAlpha
                );
        }
        else
        {
            const float LocalAlpha =
                (Alpha - 0.35f) / 0.65f;

            Scale =
                FMath::Lerp(
                    1.15f,
                    0.65f,
                    LocalAlpha
                );
        }

        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );


        const float FadeAlpha =
            FMath::Clamp(
                (Alpha - 0.25f) / 0.75f,
                0.0f,
                1.0f
            );

        SetRenderOpacity(
            1.0f - FadeAlpha
        );


        if (Alpha >= 1.0f)
        {
            bAnimatingSuccess = false;

            SetVisibility(
                ESlateVisibility::Collapsed
            );

            OnNumberCellAnimationFinished.Broadcast(
                CellIndex,
                true
            );
        }

        return;
    }


    // ========================================================
    // WRONG
    // ========================================================

    if (bAnimatingWrong)
    {
        AnimationTime += InDeltaTime;

        const float Duration =
            FMath::Max(
                0.01f,
                WrongAnimationDuration
            );

        const float Alpha =
            FMath::Clamp(
                AnimationTime / Duration,
                0.0f,
                1.0f
            );


        const float Strength =
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
                Oscillation * Strength,
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


            OnNumberCellAnimationFinished.Broadcast(
                CellIndex,
                false
            );
        }
    }
}