// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "Slate/SlateBrushAsset.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "CPPW_WiresPuzzleBase.generated.h"

/**
 * 
 */
UCLASS()
class VR_GAMES_HEADLESS_API UCPPW_WiresPuzzleBase : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	TArray<int32> Grid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	TArray<int32> TempRow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	int32 StartIndex = 0;

	UFUNCTION(BlueprintCallable)
	void MoveRow(int32 RowIndex, int32 Direction);

	UFUNCTION(BlueprintCallable)
	void MoveColumn(int32 ColumnIndex, int32 Direction);

	UFUNCTION(BlueprintCallable)
	void UpdateGridUI();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	bool CheckWin() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle")
	void OnPuzzleCompleted();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* BallTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* HoleTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* BallInHoleTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* EmptyCellTexture;

protected:

	virtual void NativeConstruct() override;

private:

	void MoveBallInRow(int32 RowIndex, int32 CurrentIndex, int32 Direction);
	void MoveBallInColumn(int32 ColumnIndex, int32 CurrentRow, int32 Direction);

	bool HasHorizontalWall(int32 RowIndex, int32 A, int32 B) const;
	bool HasVerticalWall(int32 ColumnIndex, int32 ARow, int32 BRow) const;

	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_0;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_1;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_2;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_3;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_4;

	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_0;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_1;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_2;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_3;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_4;

	UPROPERTY(meta = (BindWidget)) UBorder* Cell_2_0;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_2_1;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_2_2;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_2_3;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_2_4;

	UPROPERTY(meta = (BindWidget)) UBorder* Cell_3_0;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_3_1;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_3_2;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_3_3;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_3_4;

	UPROPERTY(meta = (BindWidget)) UBorder* Cell_4_0;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_4_1;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_4_2;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_4_3;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_4_4;

	TArray<UBorder*> Cells;
};