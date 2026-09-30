#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "CPPW_LightsOutGame.generated.h"

UCLASS()
class VR_GAMES_HEADLESS_API UCPPW_LightsOutGame : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lights Out")
	TArray<int32> Grid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lights Out")
	bool bPuzzleCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lights Out|Colors")
	FLinearColor OnColor = FLinearColor(1.0f, 0.85f, 0.05f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lights Out|Colors")
	FLinearColor OffColor = FLinearColor(0.02f, 0.02f, 0.04f, 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Lights Out")
	void OnCellClicked(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Lights Out")
	void ResetPuzzle();

	UFUNCTION(BlueprintCallable, Category = "Lights Out")
	void UpdateGridUI();

	UFUNCTION(BlueprintCallable, Category = "Lights Out")
	bool IsPuzzleSolved() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Lights Out")
	void OnPuzzleCompleted();

protected:

	virtual void NativeConstruct() override;

private:

	static constexpr int32 Width = 5;
	static constexpr int32 Height = 5;
	static constexpr int32 CellCount = Width * Height;

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

	void ToggleCell(int32 Index);
	void ToggleRowCol(int32 Row, int32 Col);
	bool IsValidRowCol(int32 Row, int32 Col) const;
	void ApplyCellVisual(int32 Index);
};