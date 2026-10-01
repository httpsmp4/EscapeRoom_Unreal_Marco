#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "CPPW_GratePuzzleGame.generated.h"

UCLASS()
class VR_GAMES_HEADLESS_API UCPPW_GratePuzzleGame : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grate Puzzle")
	int32 CurrentStep = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grate Puzzle")
	bool bPuzzleCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grate Puzzle|Colors")
	FLinearColor NormalColor = FLinearColor(0.15f, 0.15f, 0.15f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grate Puzzle|Colors")
	FLinearColor ActiveColor = FLinearColor(0.9f, 0.75f, 0.25f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grate Puzzle|Colors")
	FLinearColor CompletedColor = FLinearColor(0.1f, 0.8f, 0.35f, 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Grate Puzzle")
	void OnCellClicked(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Grate Puzzle")
	void ResetPuzzle();

	UFUNCTION(BlueprintCallable, Category = "Grate Puzzle")
	void UpdateGridUI();

	UFUNCTION(BlueprintCallable, Category = "Grate Puzzle")
	bool IsPuzzleSolved() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Grate Puzzle")
	void OnPuzzleCompleted();

protected:

	virtual void NativeConstruct() override;

private:

	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_0;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_1;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_0_2;

	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_0;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_1;
	UPROPERTY(meta = (BindWidget)) UBorder* Cell_1_2;

	TArray<UBorder*> Cells;
	TArray<int32> SolutionSequence;

	void ApplyCellVisual(int32 Index);
	void SetCellColor(UBorder* Cell, const FLinearColor& Color);
};