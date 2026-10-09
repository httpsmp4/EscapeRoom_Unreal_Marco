// Fill out your copyright notice in the Description page of Project Settings.

#include "ArrowPuzzleCellWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"


void UArrowPuzzleCellWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Click)
    {
        Button_Click->OnClicked.RemoveDynamic(
            this,
            &UArrowPuzzleCellWidget::HandleButtonClicked
        );

        Button_Click->OnClicked.AddDynamic(
            this,
            &UArrowPuzzleCellWidget::HandleButtonClicked
        );

        Button_Click->SetIsEnabled(true);
    }

    SetRenderTransformPivot(
        FVector2D(0.5f, 0.5f)
    );

    UpdateArrowRotation();
}


// ============================================================
// INITIALIZE
// ============================================================

void UArrowPuzzleCellWidget::InitializeCell(
    int32 NewCellIndex
)
{
    CellIndex = NewCellIndex;

    ResetCell();
}


// ============================================================
// RESET
// ============================================================

void UArrowPuzzleCellWidget::ResetCell()
{
    bActive = bStartsActive;
    bAvailable = false;

    bAnimatingRemoval = false;
    bAnimatingBlocked = false;

    AnimationTime = 0.0f;

    SetRenderOpacity(1.0f);

    SetRenderTranslation(
        FVector2D::ZeroVector
    );

    SetRenderScale(
        FVector2D(1.0f, 1.0f)
    );

    SetVisibility(
        bActive
        ? ESlateVisibility::Visible
        : ESlateVisibility::Collapsed
    );

    if (Button_Click)
    {
        Button_Click->SetIsEnabled(
            bActive
        );
    }

    UpdateArrowRotation();

    SetAvailable(false);
}


// ============================================================
// AVAILABLE
// ============================================================

void UArrowPuzzleCellWidget::SetAvailable(
    bool bNewAvailable
)
{
    bAvailable = bNewAvailable;

    if (!Image_Cell)
    {
        return;
    }

    if (!bActive)
    {
        return;
    }

    if (
        bAvailable &&
        AvailableCellTexture
        )
    {
        Image_Cell->SetBrushFromTexture(
            AvailableCellTexture,
            true
        );
    }
    else if (NormalCellTexture)
    {
        Image_Cell->SetBrushFromTexture(
            NormalCellTexture,
            true
        );
    }
}


// ============================================================
// CLICK
// ============================================================

void UArrowPuzzleCellWidget::HandleButtonClicked()
{
    if (!bActive)
    {
        return;
    }

    if (IsAnimating())
    {
        return;
    }

    OnArrowCellClicked.Broadcast(
        CellIndex
    );
}


// ============================================================
// IS ANIMATING
// ============================================================

bool UArrowPuzzleCellWidget::IsAnimating() const
{
    return
        bAnimatingRemoval ||
        bAnimatingBlocked;
}


// ============================================================
// REMOVE ANIMATION
// ============================================================

void UArrowPuzzleCellWidget::BeginRemoveAnimation()
{
    if (!bActive)
    {
        return;
    }

    if (IsAnimating())
    {
        return;
    }

    /*
     * La cella smette immediatamente di esistere
     * nella logica del puzzle.
     *
     * Graficamente rimane fino alla fine del volo.
     */
    bActive = false;

    bAnimatingRemoval = true;
    bAnimatingBlocked = false;

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
        Button_Click->SetIsEnabled(false);
    }
}


// ============================================================
// BLOCKED ANIMATION
// ============================================================

void UArrowPuzzleCellWidget::BeginBlockedAnimation()
{
    if (!bActive)
    {
        return;
    }

    if (IsAnimating())
    {
        return;
    }

    bAnimatingBlocked = true;
    bAnimatingRemoval = false;

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
        Button_Click->SetIsEnabled(false);
    }
}


// ============================================================
// TICK
// ============================================================

void UArrowPuzzleCellWidget::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime
)
{
    Super::NativeTick(
        MyGeometry,
        InDeltaTime
    );

    // ========================================================
    // REMOVE
    // ========================================================

    if (bAnimatingRemoval)
    {
        AnimationTime += InDeltaTime;

        const float Duration =
            FMath::Max(
                RemoveAnimationDuration,
                0.01f
            );

        const float Alpha =
            FMath::Clamp(
                AnimationTime / Duration,
                0.0f,
                1.0f
            );

        const float SmoothAlpha =
            1.0f -
            FMath::Pow(
                1.0f - Alpha,
                3.0f
            );

        const FVector2D CellSize =
            MyGeometry.GetLocalSize();

        const float BaseDistance =
            FMath::Max(
                CellSize.X,
                CellSize.Y
            );

        const float Distance =
            BaseDistance *
            RemoveDistanceMultiplier;

        const FVector2D Direction =
            DirectionToVector(
                StartDirection
            );

        SetRenderTranslation(
            Direction *
            Distance *
            SmoothAlpha
        );

        const float Scale =
            FMath::Lerp(
                1.0f,
                0.72f,
                SmoothAlpha
            );

        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );

        /*
         * Il fade parte dopo il movimento iniziale.
         */
        const float FadeAlpha =
            FMath::Clamp(
                (Alpha - 0.35f) / 0.65f,
                0.0f,
                1.0f
            );

        SetRenderOpacity(
            1.0f - FadeAlpha
        );

        if (Alpha >= 1.0f)
        {
            bAnimatingRemoval = false;

            SetRenderOpacity(0.0f);

            SetVisibility(
                ESlateVisibility::Collapsed
            );

            /*
             * NOME CORRETTO.
             */
            OnCellAnimationFinished.Broadcast(
                CellIndex,
                true
            );
        }

        return;
    }


    // ========================================================
    // BLOCKED SHAKE
    // ========================================================

    if (bAnimatingBlocked)
    {
        AnimationTime += InDeltaTime;

        const float Duration =
            FMath::Max(
                BlockedAnimationDuration,
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
            Pulse * 0.08f;

        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );

        if (Alpha >= 1.0f)
        {
            bAnimatingBlocked = false;

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

            /*
             * NOME CORRETTO.
             */
            OnCellAnimationFinished.Broadcast(
                CellIndex,
                false
            );
        }
    }
}


// ============================================================
// ROTATION
// ============================================================

float UArrowPuzzleCellWidget::DirectionToAngle(
    EArrowDirection Direction
) const
{
    switch (Direction)
    {
    case EArrowDirection::Up:
        return 0.0f;

    case EArrowDirection::Right:
        return 90.0f;

    case EArrowDirection::Down:
        return 180.0f;

    case EArrowDirection::Left:
        return 270.0f;
    }

    return 0.0f;
}


// ============================================================
// VECTOR
// ============================================================

FVector2D UArrowPuzzleCellWidget::DirectionToVector(
    EArrowDirection Direction
) const
{
    switch (Direction)
    {
    case EArrowDirection::Up:
        return FVector2D(0.0f, -1.0f);

    case EArrowDirection::Right:
        return FVector2D(1.0f, 0.0f);

    case EArrowDirection::Down:
        return FVector2D(0.0f, 1.0f);

    case EArrowDirection::Left:
        return FVector2D(-1.0f, 0.0f);
    }

    return FVector2D::ZeroVector;
}


// ============================================================
// UPDATE ROTATION
// ============================================================

void UArrowPuzzleCellWidget::UpdateArrowRotation()
{
    if (!Image_Arrow)
    {
        return;
    }

    Image_Arrow->SetRenderTransformPivot(
        FVector2D(
            0.5f,
            0.5f
        )
    );

    Image_Arrow->SetRenderTransformAngle(
        DirectionToAngle(
            StartDirection
        )
    );
}