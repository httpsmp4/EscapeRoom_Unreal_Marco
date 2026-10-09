// Fill out your copyright notice in the Description page of Project Settings.
#include "MemoryPairCellWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"


void UMemoryPairCellWidget::NativeConstruct()
{
    Super::NativeConstruct();


    SetRenderTransformPivot(
        FVector2D(0.5f, 0.5f)
    );


    if (Image_Card)
    {
        Image_Card->SetVisibility(
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
            &UMemoryPairCellWidget::HandleButtonClicked
        );


        Button_Click->OnClicked.AddDynamic(
            this,
            &UMemoryPairCellWidget::HandleButtonClicked
        );
    }


    ResetCard();
}


// ============================================================
// INITIALIZE
// ============================================================

void UMemoryPairCellWidget::InitializeCell(
    int32 NewCellIndex
)
{
    CellIndex =
        NewCellIndex;
}


// ============================================================
// CONFIGURE
// ============================================================

void UMemoryPairCellWidget::ConfigureCard(
    int32 NewPairId,
    UTexture2D* NewFrontTexture,
    UTexture2D* NewBackTexture
)
{
    PairId =
        NewPairId;


    FrontTexture =
        NewFrontTexture;


    BackTexture =
        NewBackTexture;


    ResetCard();


    SetVisibility(
        ESlateVisibility::Visible
    );
}


// ============================================================
// RESET
// ============================================================

void UMemoryPairCellWidget::ResetCard()
{
    CardState =
        EMemoryCardState::Hidden;


    bAnimatingFlip =
        false;


    bAnimatingMatchPulse =
        false;


    bFlipTargetFaceUp =
        false;


    bTextureSwapped =
        false;


    AnimationTime =
        0.0f;


    SetRenderScale(
        FVector2D(
            1.0f,
            1.0f
        )
    );


    SetRenderOpacity(
        1.0f
    );


    if (Image_Card)
    {
        Image_Card->SetBrushFromTexture(
            BackTexture,
            true
        );
    }


    if (Button_Click)
    {
        Button_Click->SetIsEnabled(
            true
        );
    }
}


// ============================================================
// CLICK
// ============================================================

void UMemoryPairCellWidget::HandleButtonClicked()
{
    if (
        CardState !=
        EMemoryCardState::Hidden
        )
    {
        return;
    }


    if (IsAnimating())
    {
        return;
    }


    OnMemoryCardClicked.Broadcast(
        CellIndex
    );
}


// ============================================================
// REVEAL
// ============================================================

void UMemoryPairCellWidget::RevealCard()
{
    if (
        CardState !=
        EMemoryCardState::Hidden
        )
    {
        return;
    }


    CardState =
        EMemoryCardState::Revealed;


    StartFlip(
        true
    );
}


// ============================================================
// HIDE
// ============================================================

void UMemoryPairCellWidget::HideCard()
{
    if (
        CardState !=
        EMemoryCardState::Revealed
        )
    {
        return;
    }


    CardState =
        EMemoryCardState::Hidden;


    StartFlip(
        false
    );
}


// ============================================================
// MATCH
// ============================================================

void UMemoryPairCellWidget::SetMatched()
{
    CardState =
        EMemoryCardState::Matched;


    if (Button_Click)
    {
        Button_Click->SetIsEnabled(
            false
        );
    }


    bAnimatingMatchPulse =
        true;


    AnimationTime =
        0.0f;
}


// ============================================================
// FLIP
// ============================================================

void UMemoryPairCellWidget::StartFlip(
    bool bNewFaceUp
)
{
    bAnimatingFlip =
        true;


    bFlipTargetFaceUp =
        bNewFaceUp;


    bTextureSwapped =
        false;


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
// IS ANIMATING
// ============================================================

bool UMemoryPairCellWidget::IsAnimating() const
{
    return
        bAnimatingFlip ||
        bAnimatingMatchPulse;
}


// ============================================================
// TICK
// ============================================================

void UMemoryPairCellWidget::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime
)
{
    Super::NativeTick(
        MyGeometry,
        InDeltaTime
    );


    (void)MyGeometry;


    // ========================================================
    // FLIP
    // ========================================================

    if (bAnimatingFlip)
    {
        AnimationTime +=
            InDeltaTime;


        const float Duration =
            FMath::Max(
                0.01f,
                FlipDuration
            );


        const float Alpha =
            FMath::Clamp(
                AnimationTime / Duration,
                0.0f,
                1.0f
            );


        float ScaleX;


        if (Alpha < 0.5f)
        {
            ScaleX =
                FMath::Lerp(
                    1.0f,
                    0.0f,
                    Alpha * 2.0f
                );
        }
        else
        {
            if (!bTextureSwapped)
            {
                bTextureSwapped =
                    true;


                if (Image_Card)
                {
                    Image_Card->SetBrushFromTexture(
                        bFlipTargetFaceUp
                        ? FrontTexture
                        : BackTexture,
                        true
                    );
                }
            }


            ScaleX =
                FMath::Lerp(
                    0.0f,
                    1.0f,
                    (Alpha - 0.5f) * 2.0f
                );
        }


        SetRenderScale(
            FVector2D(
                ScaleX,
                1.0f
            )
        );


        if (Alpha >= 1.0f)
        {
            bAnimatingFlip =
                false;


            SetRenderScale(
                FVector2D(
                    1.0f,
                    1.0f
                )
            );


            if (
                Button_Click &&
                CardState ==
                EMemoryCardState::Hidden
                )
            {
                Button_Click->SetIsEnabled(
                    true
                );
            }


            OnMemoryCardFlipFinished.Broadcast(
                CellIndex,
                bFlipTargetFaceUp
            );
        }


        return;
    }


    // ========================================================
    // MATCH PULSE
    // ========================================================

    if (bAnimatingMatchPulse)
    {
        AnimationTime +=
            InDeltaTime;


        const float Duration =
            FMath::Max(
                0.01f,
                MatchPulseDuration
            );


        const float Alpha =
            FMath::Clamp(
                AnimationTime / Duration,
                0.0f,
                1.0f
            );


        const float Pulse =
            FMath::Sin(
                Alpha * PI
            );


        const float Scale =
            1.0f +
            Pulse * 0.15f;


        SetRenderScale(
            FVector2D(
                Scale,
                Scale
            )
        );


        if (Alpha >= 1.0f)
        {
            bAnimatingMatchPulse =
                false;


            SetRenderScale(
                FVector2D(
                    1.0f,
                    1.0f
                )
            );
        }
    }
}