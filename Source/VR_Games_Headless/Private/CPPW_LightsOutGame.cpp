#include "CPPW_LightsOutGame.h"

void UCPPW_LightsOutGame::NativeConstruct()
{
	Super::NativeConstruct();

	Cells.Empty();

	Cells.Add(Cell_0_0);
	Cells.Add(Cell_0_1);
	Cells.Add(Cell_0_2);
	Cells.Add(Cell_0_3);
	Cells.Add(Cell_0_4);

	Cells.Add(Cell_1_0);
	Cells.Add(Cell_1_1);
	Cells.Add(Cell_1_2);
	Cells.Add(Cell_1_3);
	Cells.Add(Cell_1_4);

	Cells.Add(Cell_2_0);
	Cells.Add(Cell_2_1);
	Cells.Add(Cell_2_2);
	Cells.Add(Cell_2_3);
	Cells.Add(Cell_2_4);

	Cells.Add(Cell_3_0);
	Cells.Add(Cell_3_1);
	Cells.Add(Cell_3_2);
	Cells.Add(Cell_3_3);
	Cells.Add(Cell_3_4);

	Cells.Add(Cell_4_0);
	Cells.Add(Cell_4_1);
	Cells.Add(Cell_4_2);
	Cells.Add(Cell_4_3);
	Cells.Add(Cell_4_4);

	ResetPuzzle();
}

void UCPPW_LightsOutGame::ResetPuzzle()
{
	Grid.Init(0, CellCount);

	// Configurazione iniziale semplice e risolvibile
	Grid[2] = 1;
	Grid[6] = 1;
	Grid[7] = 1;
	Grid[8] = 1;
	Grid[12] = 1;
	Grid[16] = 1;
	Grid[17] = 1;
	Grid[18] = 1;
	Grid[22] = 1;

	bPuzzleCompleted = false;

	UpdateGridUI();
}

void UCPPW_LightsOutGame::OnCellClicked(int32 Index)
{
	if (bPuzzleCompleted) return;
	if (!Grid.IsValidIndex(Index)) return;

	const int32 Row = Index / Width;
	const int32 Col = Index % Width;

	ToggleRowCol(Row, Col);
	ToggleRowCol(Row - 1, Col);
	ToggleRowCol(Row + 1, Col);
	ToggleRowCol(Row, Col - 1);
	ToggleRowCol(Row, Col + 1);

	UpdateGridUI();

	if (IsPuzzleSolved())
	{
		bPuzzleCompleted = true;

		UE_LOG(LogTemp, Warning, TEXT("LIGHTS OUT COMPLETED"));

		OnPuzzleCompleted();
	}
}

void UCPPW_LightsOutGame::ToggleCell(int32 Index)
{
	if (!Grid.IsValidIndex(Index)) return;

	Grid[Index] = Grid[Index] == 0 ? 1 : 0;
}

void UCPPW_LightsOutGame::ToggleRowCol(int32 Row, int32 Col)
{
	if (!IsValidRowCol(Row, Col)) return;

	const int32 Index = Row * Width + Col;
	ToggleCell(Index);
}

bool UCPPW_LightsOutGame::IsValidRowCol(int32 Row, int32 Col) const
{
	return Row >= 0 && Row < Height && Col >= 0 && Col < Width;
}

bool UCPPW_LightsOutGame::IsPuzzleSolved() const
{
	for (const int32 Value : Grid)
	{
		if (Value != 0)
		{
			return false;
		}
	}

	return true;
}

void UCPPW_LightsOutGame::UpdateGridUI()
{
	for (int32 i = 0; i < Cells.Num(); i++)
	{
		if (!Cells.IsValidIndex(i) || !Cells[i]) continue;

		ApplyCellVisual(i);
	}
}

void UCPPW_LightsOutGame::ApplyCellVisual(int32 Index)
{
	if (!Cells.IsValidIndex(Index) || !Cells[Index]) return;
	if (!Grid.IsValidIndex(Index)) return;

	const FLinearColor Color = Grid[Index] == 1 ? OnColor : OffColor;

	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Box;

	Cells[Index]->SetBrush(Brush);
	Cells[Index]->SetBrushColor(Color);
}