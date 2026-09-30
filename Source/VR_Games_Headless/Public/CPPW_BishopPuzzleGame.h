#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Engine/Texture2D.h"
#include "CPPW_BishopPuzzleGame.generated.h"

UCLASS()
class VR_GAMES_HEADLESS_API UCPPW_BishopPuzzleGame : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bishop Puzzle")
	TArray<int32> Grid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bishop Puzzle")
	int32 SelectedIndex = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bishop Puzzle")
	bool bHasSelection = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Bishop Puzzle")
	bool bPuzzleCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bishop Puzzle|Textures")
	UTexture2D* BlackBishopTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bishop Puzzle|Textures")
	UTexture2D* WhiteBishopTexture = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Bishop Puzzle")
	void OnCellClicked(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Bishop Puzzle")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category = "Bishop Puzzle")
	void UpdateGridUI();

	UFUNCTION(BlueprintCallable, Category = "Bishop Puzzle")
	bool IsValidBishopMove(int32 From, int32 To) const;

	UFUNCTION(BlueprintCallable, Category = "Bishop Puzzle")
	bool IsPuzzleSolved() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Bishop Puzzle")
	void OnPuzzleCompleted();

protected:

	virtual void NativeConstruct() override;

private:

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

	TArray<UBorder*> Cells;

	bool WouldBeCapturedAfterMove(int32 From, int32 To) const;

	bool IsSquareAttackedByEnemyBishop(
		int32 SquareIndex,
		int32 PieceColor,
		const TArray<int32>& Board
	) const;

	void ApplyCellVisual(int32 Index, int32 GridValue);

	void SetCellTexture(
		UBorder* Cell,
		UTexture2D* Texture,
		const FLinearColor& Tint
	);

	void SetCellColor(
		UBorder* Cell,
		const FLinearColor& Color
	);
};