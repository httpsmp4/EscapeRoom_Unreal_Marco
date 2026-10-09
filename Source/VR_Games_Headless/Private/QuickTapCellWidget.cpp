#include "QuickTapCellWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"


// ============================================================
// CONSTRUCT
// ============================================================

void UQuickTapCellWidget::NativeConstruct()
{
    Super::NativeConstruct();


    SetRenderTransformPivot(
        FVector2D(0.5f, 0.5f)
    );


    if (Border_Color)
    {
        Border_Color->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (Image_Frame)
    {
        Image_Frame->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );
    }


    if (Button_Click)
    {
        Button_Click->OnClicked.RemoveDynamic(
            this,
            &UQuickTapCellWidget::HandleButtonClicked
        );


        Button_Click->OnClicked.AddDynamic(
            this,
            &UQuickTapCellWidget::HandleButtonClicked
        );
    }


    ResetVisualState();
}


// ============================================================
// INITIALIZE
// ============================================================

void UQuickTapCellWidget::InitializeCell(
    int32 NewCellIndex
)
{
    CellIndex = NewCellIndex;

    ResetVisualState();
}


// ============================================================
// CONFIGURE
// ============================================================

void UQuickTapCellWidget::ConfigureCell(
    const FLinearColor& NewColor,
    bool bNewIsTarget,
    bool bNewActive
)
{
    bIsTarget =
        bNewIsTarget;


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

void UQuickTapCellWidget::SetCellColor(
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

void UQuickTapCellWidget::SetCellActive(
    bool bNewActive
)
{
    bActive =
        bNewActive;


    if (bActive)
    {
        SetVisibility(
            ESlateVisibility::Visible
        );


        SetRenderOpacity(
            1.0f
        );


        if (Button_Click)
        {
            Button_Click->SetIsEnabled(
                true
            );
        }
    }
    else
    {
        if (Button_Click)
        {
            Button_Click->SetIsEnabled(
                false
            );
        }


        SetVisibility(
            ESlateVisibility::Collapsed
        );
    }
}


// ============================================================
// RESET VISUAL
// ============================================================

void UQuickTapCellWidget::ResetVisualState()
{
    ActiveAnimation =
        EQuickTapCellAnimationType::None;


    AnimationTime =
        0.0f;


    SetRenderOpacity(
        1.0f
    );


    SetRenderTranslation(
        FVector2D::ZeroVector
    );


    SetRenderScale(
        FVector2D(
            1.0f,
            1.0f
        )
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

void UQuickTapCellWidget::HandleButtonClicked()
{
    if (!bActive)
    {
        return;
    }


    if (IsAnimating())
    {
        return;
    }


    OnQuickTapCellClicked.Broadcast(
        CellIndex
    );
}


// ============================================================
// IS ANIMATING
// ============================================================

bool UQuickTapCellWidget::IsAnimating() const
{
    return
        ActiveAnimation !=
        EQuickTapCellAnimationType::None;
}


// ============================================================
// SUCCESS
// ============================================================

void UQuickTapCellWidget::PlaySuccessAnimation()
{
    if (!bActive || IsAnimating())
    {
        return;
    }


    ActiveAnimation =
        EQuickTapCellAnimationType::Success;


    AnimationTime =
        0.0f;


    if (Button_Click)
    {
        Button_Click->SetIsEnabled(
            false
        );
    }
}


// ============================================================
// WRONG
// ============================================================

void UQuickTapCellWidget::PlayWrongAnimation()
{
    if (!bActive || IsAnimating())
    {
        return;
    }


    ActiveAnimation =
        EQuickTapCellAnimationType::Wrong;


    AnimationTime =
        0.0f;


    if (Button_Click)
    {
        Button_Click->SetIsEnabled(
            false
        );
    }
}


// ============================================================
// MISS
// ============================================================

void UQuickTapCellWidget::PlayMissAnimation()
{
    if (!bActive || IsAnimating())
    {
        return;
    }


    ActiveAnimation =
        EQuickTapCellAnimationType::Miss;


    AnimationTime =
        0.0f;


    if (Button_Click)
    {
        Button_Click->SetIsEnabled(
            false
        );
    }
}


// ============================================================
// TICK
// ============================================================

void UQuickTapCellWidget::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime
)
{
    Super::NativeTick(
        MyGeometry,
        InDeltaTime
    );


    /*
     * Evita warning per parametro inutilizzato.
     */
    (void)MyGeometry;


    if (
        ActiveAnimation ==
        EQuickTapCellAnimationType::None
        )
    {
        return;
    }


    AnimationTime +=
        InDeltaTime;


    // ========================================================
    // SUCCESS
    // ========================================================

    if (
        ActiveAnimation ==
        EQuickTapCellAnimationType::Success
        )
    {
        const float Duration =
            FMath::Max(
                0.01f,
                SuccessAnimationDuration
            );


        const float Alpha =
            FMath::Clamp(
                AnimationTime /
                Duration,
                0.0f,
                1.0f
            );


        const float Pulse =
            FMath::Sin(
                Alpha * PI
            );


        const float Scale =
            1.0f +
            Pulse * 0.25f;


        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );


        /*
         * Piccolo flash.
         */
        SetRenderOpacity(
            FMath::Lerp(
                1.0f,
                0.70f,
                Pulse
            )
        );


        if (Alpha >= 1.0f)
        {
            ActiveAnimation =
                EQuickTapCellAnimationType::None;


            SetRenderScale(
                FVector2D(
                    1.0f,
                    1.0f
                )
            );


            SetRenderOpacity(
                1.0f
            );


            if (Button_Click && bActive)
            {
                Button_Click->SetIsEnabled(
                    true
                );
            }


            OnQuickTapCellAnimationFinished.Broadcast(
                CellIndex,
                EQuickTapCellAnimationType::Success
            );
        }


        return;
    }


    // ========================================================
    // WRONG
    // ========================================================

    if (
        ActiveAnimation ==
        EQuickTapCellAnimationType::Wrong
        )
    {
        const float Duration =
            FMath::Max(
                0.01f,
                WrongAnimationDuration
            );


        const float Alpha =
            FMath::Clamp(
                AnimationTime /
                Duration,
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
                Oscillation *
                Strength,
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
            ActiveAnimation =
                EQuickTapCellAnimationType::None;


            SetRenderTranslation(
                FVector2D::ZeroVector
            );


            SetRenderScale(
                FVector2D(
                    1.0f,
                    1.0f
                )
            );


            if (Button_Click && bActive)
            {
                Button_Click->SetIsEnabled(
                    true
                );
            }


            OnQuickTapCellAnimationFinished.Broadcast(
                CellIndex,
                EQuickTapCellAnimationType::Wrong
            );
        }


        return;
    }


    // ========================================================
    // MISS
    // ========================================================

    if (
        ActiveAnimation ==
        EQuickTapCellAnimationType::Miss
        )
    {
        const float Duration =
            FMath::Max(
                0.01f,
                MissAnimationDuration
            );


        const float Alpha =
            FMath::Clamp(
                AnimationTime /
                Duration,
                0.0f,
                1.0f
            );


        const float Pulse =
            FMath::Sin(
                Alpha *
                PI *
                2.0f
            );


        const float Scale =
            1.0f +
            FMath::Abs(Pulse) *
            0.12f;


        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );


        SetRenderOpacity(
            FMath::Lerp(
                1.0f,
                0.45f,
                FMath::Abs(Pulse)
            )
        );


        if (Alpha >= 1.0f)
        {
            ActiveAnimation =
                EQuickTapCellAnimationType::None;


            SetRenderScale(
                FVector2D(
                    1.0f,
                    1.0f
                )
            );


            SetRenderOpacity(
                1.0f
            );


            if (Button_Click && bActive)
            {
                Button_Click->SetIsEnabled(
                    true
                );
            }


            OnQuickTapCellAnimationFinished.Broadcast(
                CellIndex,
                EQuickTapCellAnimationType::Miss
            );
        }
    }
}