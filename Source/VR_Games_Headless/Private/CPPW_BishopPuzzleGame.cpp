#include "CPPW_BishopPuzzleGame.h"

void UCPPW_BishopPuzzleGame::NativeConstruct()
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

	Grid.Init(0, 20);

	// 0 = vuoto
	// 1 = alfiere nero
	// 2 = alfiere bianco

	Grid[0 * 5 + 0] = 1;
	Grid[1 * 5 + 0] = 1;
	Grid[2 * 5 + 0] = 1;
	Grid[3 * 5 + 0] = 1;

	Grid[0 * 5 + 4] = 2;
	Grid[1 * 5 + 4] = 2;
	Grid[2 * 5 + 4] = 2;
	Grid[3 * 5 + 4] = 2;

	SelectedIndex = -1;
	bHasSelection = false;
	bPuzzleCompleted = false;

	UpdateGridUI();
}

void UCPPW_BishopPuzzleGame::OnCellClicked(int32 Index)
{
	if (bPuzzleCompleted)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Clicked Cell: %d"), Index);

	if (!Grid.IsValidIndex(Index))
	{
		ClearSelection();
		return;
	}

	if (!bHasSelection)
	{
		if (Grid[Index] == 1 || Grid[Index] == 2)
		{
			SelectedIndex = Index;
			bHasSelection = true;

			UE_LOG(LogTemp, Warning, TEXT("Selected Bishop at: %d"), SelectedIndex);

			UpdateGridUI();
		}

		return;
	}

	if (Index == SelectedIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deselected"));
		ClearSelection();
		return;
	}

	if (IsValidBishopMove(SelectedIndex, Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("Move Bishop from %d to %d"), SelectedIndex, Index);

		Grid[Index] = Grid[SelectedIndex];
		Grid[SelectedIndex] = 0;

		if (!bPuzzleCompleted && IsPuzzleSolved())
		{
			bPuzzleCompleted = true;

			UE_LOG(LogTemp, Warning, TEXT("BISHOP PUZZLE COMPLETED"));

			UpdateGridUI();
			OnPuzzleCompleted();
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid move from %d to %d"), SelectedIndex, Index);
	}

	ClearSelection();
}

void UCPPW_BishopPuzzleGame::ClearSelection()
{
	SelectedIndex = -1;
	bHasSelection = false;

	UpdateGridUI();
}

bool UCPPW_BishopPuzzleGame::IsValidBishopMove(int32 From, int32 To) const
{
	if (!Grid.IsValidIndex(From)) return false;
	if (!Grid.IsValidIndex(To)) return false;

	if (Grid[From] != 1 && Grid[From] != 2) return false;
	if (Grid[To] != 0) return false;

	const int32 Width = 5;

	const int32 FromRow = From / Width;
	const int32 FromCol = From % Width;

	const int32 ToRow = To / Width;
	const int32 ToCol = To % Width;

	const int32 RowDiff = ToRow - FromRow;
	const int32 ColDiff = ToCol - FromCol;

	if (FMath::Abs(RowDiff) != FMath::Abs(ColDiff))
	{
		return false;
	}

	const int32 StepRow = RowDiff > 0 ? 1 : -1;
	const int32 StepCol = ColDiff > 0 ? 1 : -1;

	int32 CheckRow = FromRow + StepRow;
	int32 CheckCol = FromCol + StepCol;

	while (CheckRow != ToRow || CheckCol != ToCol)
	{
		const int32 CheckIndex = CheckRow * Width + CheckCol;

		if (!Grid.IsValidIndex(CheckIndex)) return false;
		if (Grid[CheckIndex] != 0) return false;

		CheckRow += StepRow;
		CheckCol += StepCol;
	}

	return !WouldBeCapturedAfterMove(From, To);
}

bool UCPPW_BishopPuzzleGame::WouldBeCapturedAfterMove(int32 From, int32 To) const
{
	if (!Grid.IsValidIndex(From)) return true;
	if (!Grid.IsValidIndex(To)) return true;

	const int32 PieceColor = Grid[From];

	if (PieceColor != 1 && PieceColor != 2)
	{
		return true;
	}

	TArray<int32> SimulatedGrid = Grid;

	SimulatedGrid[To] = SimulatedGrid[From];
	SimulatedGrid[From] = 0;

	return IsSquareAttackedByEnemyBishop(To, PieceColor, SimulatedGrid);
}

bool UCPPW_BishopPuzzleGame::IsSquareAttackedByEnemyBishop(
	int32 SquareIndex,
	int32 PieceColor,
	const TArray<int32>& Board
) const
{
	const int32 Width = 5;

	if (!Board.IsValidIndex(SquareIndex)) return true;

	const int32 TargetRow = SquareIndex / Width;
	const int32 TargetCol = SquareIndex % Width;

	const int32 EnemyColor = PieceColor == 1 ? 2 : 1;

	for (int32 EnemyIndex = 0; EnemyIndex < Board.Num(); EnemyIndex++)
	{
		if (Board[EnemyIndex] != EnemyColor)
		{
			continue;
		}

		const int32 EnemyRow = EnemyIndex / Width;
		const int32 EnemyCol = EnemyIndex % Width;

		const int32 RowDiff = TargetRow - EnemyRow;
		const int32 ColDiff = TargetCol - EnemyCol;

		if (FMath::Abs(RowDiff) != FMath::Abs(ColDiff))
		{
			continue;
		}

		const int32 StepRow = RowDiff > 0 ? 1 : -1;
		const int32 StepCol = ColDiff > 0 ? 1 : -1;

		int32 CheckRow = EnemyRow + StepRow;
		int32 CheckCol = EnemyCol + StepCol;

		bool bPathBlocked = false;

		while (CheckRow != TargetRow || CheckCol != TargetCol)
		{
			const int32 CheckIndex = CheckRow * Width + CheckCol;

			if (!Board.IsValidIndex(CheckIndex))
			{
				bPathBlocked = true;
				break;
			}

			if (Board[CheckIndex] != 0)
			{
				bPathBlocked = true;
				break;
			}

			CheckRow += StepRow;
			CheckCol += StepCol;
		}

		if (!bPathBlocked)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Square %d is attacked by enemy bishop at %d"),
				SquareIndex,
				EnemyIndex
			);

			return true;
		}
	}

	return false;
}

bool UCPPW_BishopPuzzleGame::IsPuzzleSolved() const
{
	const int32 Width = 5;

	if (Grid.Num() != 20)
	{
		return false;
	}

	int32 BlackCount = 0;
	int32 WhiteCount = 0;

	for (int32 i = 0; i < Grid.Num(); i++)
	{
		const int32 Value = Grid[i];
		const int32 Col = i % Width;

		if (Value == 1)
		{
			BlackCount++;

			if (Col != 4)
			{
				return false;
			}
		}
		else if (Value == 2)
		{
			WhiteCount++;

			if (Col != 0)
			{
				return false;
			}
		}
	}

	return BlackCount == 4 && WhiteCount == 4;
}

void UCPPW_BishopPuzzleGame::UpdateGridUI()
{
	for (int32 i = 0; i < Cells.Num(); i++)
	{
		if (!Cells.IsValidIndex(i) || !Cells[i]) continue;

		ApplyCellVisual(i, Grid[i]);
	}
}

void UCPPW_BishopPuzzleGame::ApplyCellVisual(int32 Index, int32 GridValue)
{
	if (!Cells.IsValidIndex(Index) || !Cells[Index]) return;

	UBorder* Cell = Cells[Index];

	const int32 Row = Index / 5;
	const int32 Col = Index % 5;

	const bool bSelected = bHasSelection && Index == SelectedIndex;

	if (GridValue == 1)
	{
		SetCellTexture(Cell, BlackBishopTexture, FLinearColor::White);

		if (bSelected)
		{
			Cell->SetBrushColor(FLinearColor(1.0f, 0.85f, 0.05f, 1.0f));
		}

		return;
	}

	if (GridValue == 2)
	{
		SetCellTexture(Cell, WhiteBishopTexture, bSelected ? FLinearColor::Yellow : FLinearColor::White);
		return;
	}

	const bool bWhiteSquare = ((Row + Col) % 2 == 0);

	FLinearColor Color = bWhiteSquare
		? FLinearColor(1.0f, 1.0f, 1.0f, 0.0f)
		: FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

	if (bSelected)
	{
		Color = FLinearColor::Yellow;
	}

	SetCellColor(Cell, Color);
}

void UCPPW_BishopPuzzleGame::SetCellTexture(
	UBorder* Cell,
	UTexture2D* Texture,
	const FLinearColor& Tint
)
{
	if (!Cell) return;

	FSlateBrush Brush;

	if (Texture)
	{
		Brush.SetResourceObject(Texture);
		Brush.ImageSize = FVector2D(128.0f, 128.0f);
		Brush.DrawAs = ESlateBrushDrawType::Image;
	}

	Cell->SetBrush(Brush);
	Cell->SetBrushColor(Tint);
}

void UCPPW_BishopPuzzleGame::SetCellColor(
	UBorder* Cell,
	const FLinearColor& Color
)
{
	if (!Cell) return;

	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Box;

	Cell->SetBrush(Brush);
	Cell->SetBrushColor(Color);
}