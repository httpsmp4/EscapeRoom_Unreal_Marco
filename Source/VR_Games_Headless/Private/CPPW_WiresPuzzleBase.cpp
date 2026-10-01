// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPW_WiresPuzzleBase.h"

void UCPPW_WiresPuzzleBase::NativeConstruct()
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

	if (Grid.Num() != 25)
	{
		Grid.Init(0, 25);
	}

	for (int32 i = 0; i < 25; i++)
	{
		Grid[i] = 0;
	}

	Grid[0 * 5 + 0] = 1;
	Grid[4 * 5 + 0] = 1;
	Grid[2 * 5 + 2] = 1;
	Grid[4 * 5 + 4] = 1;

	Grid[0 * 5 + 2] = 2;
	Grid[2 * 5 + 1] = 2;
	Grid[3 * 5 + 2] = 2;
	Grid[3 * 5 + 4] = 2;

	UpdateGridUI();
}

void UCPPW_WiresPuzzleBase::MoveRow(int32 RowIndex, int32 Direction)
{
	if (RowIndex < 0 || RowIndex > 4) return;
	if (Direction != -1 && Direction != 1) return;

	TempRow.Empty();

	StartIndex = RowIndex * 5;

	for (int32 i = 0; i <= 4; i++)
	{
		TempRow.Add(Grid[StartIndex + i]);
	}

	if (Direction == -1)
	{
		for (int32 CurrentIndex = 0; CurrentIndex <= 4; CurrentIndex++)
		{
			if (TempRow[CurrentIndex] == 1 || TempRow[CurrentIndex] == 3)
			{
				MoveBallInRow(RowIndex, CurrentIndex, Direction);
			}
		}
	}
	else
	{
		for (int32 CurrentIndex = 4; CurrentIndex >= 0; CurrentIndex--)
		{
			if (TempRow[CurrentIndex] == 1 || TempRow[CurrentIndex] == 3)
			{
				MoveBallInRow(RowIndex, CurrentIndex, Direction);
			}
		}
	}

	for (int32 i = 0; i <= 4; i++)
	{
		Grid[RowIndex * 5 + i] = TempRow[i];
	}

	UpdateGridUI();

	if (CheckWin())
	{
		OnPuzzleCompleted();
	}
}

void UCPPW_WiresPuzzleBase::MoveBallInRow(int32 RowIndex, int32 CurrentIndex, int32 Direction)
{
	StartIndex = CurrentIndex;

	for (int32 Step = 0; Step <= 3; Step++)
	{
		const int32 NextIndex = StartIndex + Direction;

		if (NextIndex < 0 || NextIndex > 4) return;
		if (HasHorizontalWall(RowIndex, StartIndex, NextIndex)) return;

		if (TempRow[NextIndex] == 1 || TempRow[NextIndex] == 3) return;

		const bool bLeavingTarget = TempRow[StartIndex] == 3;

		if (TempRow[NextIndex] == 2)
		{
			TempRow[StartIndex] = bLeavingTarget ? 2 : 0;
			TempRow[NextIndex] = 3;
			StartIndex = NextIndex;
			continue;
		}

		if (TempRow[NextIndex] == 0)
		{
			TempRow[StartIndex] = bLeavingTarget ? 2 : 0;
			TempRow[NextIndex] = 1;
			StartIndex = NextIndex;
			continue;
		}
	}
}

void UCPPW_WiresPuzzleBase::MoveColumn(int32 ColumnIndex, int32 Direction)
{
	if (ColumnIndex < 0 || ColumnIndex > 4) return;
	if (Direction != -1 && Direction != 1) return;

	TempRow.Empty();

	for (int32 Row = 0; Row <= 4; Row++)
	{
		TempRow.Add(Grid[Row * 5 + ColumnIndex]);
	}

	if (Direction == -1)
	{
		for (int32 CurrentRow = 0; CurrentRow <= 4; CurrentRow++)
		{
			if (TempRow[CurrentRow] == 1 || TempRow[CurrentRow] == 3)
			{
				MoveBallInColumn(ColumnIndex, CurrentRow, Direction);
			}
		}
	}
	else
	{
		for (int32 CurrentRow = 4; CurrentRow >= 0; CurrentRow--)
		{
			if (TempRow[CurrentRow] == 1 || TempRow[CurrentRow] == 3)
			{
				MoveBallInColumn(ColumnIndex, CurrentRow, Direction);
			}
		}
	}

	for (int32 Row = 0; Row <= 4; Row++)
	{
		Grid[Row * 5 + ColumnIndex] = TempRow[Row];
	}

	UpdateGridUI();

	if (CheckWin())
	{
		OnPuzzleCompleted();
	}
}

void UCPPW_WiresPuzzleBase::MoveBallInColumn(int32 ColumnIndex, int32 CurrentRow, int32 Direction)
{
	StartIndex = CurrentRow;

	for (int32 Step = 0; Step <= 3; Step++)
	{
		const int32 NextRow = StartIndex + Direction;

		if (NextRow < 0 || NextRow > 4) return;
		if (HasVerticalWall(ColumnIndex, StartIndex, NextRow)) return;

		if (TempRow[NextRow] == 1 || TempRow[NextRow] == 3) return;

		const bool bLeavingTarget = TempRow[StartIndex] == 3;

		if (TempRow[NextRow] == 2)
		{
			TempRow[StartIndex] = bLeavingTarget ? 2 : 0;
			TempRow[NextRow] = 3;
			StartIndex = NextRow;
			continue;
		}

		if (TempRow[NextRow] == 0)
		{
			TempRow[StartIndex] = bLeavingTarget ? 2 : 0;
			TempRow[NextRow] = 1;
			StartIndex = NextRow;
			continue;
		}
	}
}

bool UCPPW_WiresPuzzleBase::HasHorizontalWall(int32 RowIndex, int32 A, int32 B) const
{
	const bool Between2And3 =
		(A == 2 && B == 3) ||
		(A == 3 && B == 2);

	if (RowIndex == 0 && Between2And3) return true;
	if (RowIndex == 2 && Between2And3) return true;
	if (RowIndex == 3 && Between2And3) return true;

	return false;
}

bool UCPPW_WiresPuzzleBase::HasVerticalWall(int32 ColumnIndex, int32 ARow, int32 BRow) const
{
	const bool Between0And1 =
		(ARow == 0 && BRow == 1) ||
		(ARow == 1 && BRow == 0);

	const bool Between1And2 =
		(ARow == 1 && BRow == 2) ||
		(ARow == 2 && BRow == 1);

	const bool Between3And4 =
		(ARow == 3 && BRow == 4) ||
		(ARow == 4 && BRow == 3);

	if (ColumnIndex == 1 && Between0And1) return true;
	if (ColumnIndex == 4 && Between1And2) return true;
	if (ColumnIndex == 4 && Between3And4) return true;

	return false;
}

//void UCPPW_WiresPuzzleBase::UpdateGridUI()
//{
//	for (int32 i = 0; i < 25; i++)
//	{
//		if (!Cells.IsValidIndex(i) || !Cells[i]) continue;
//
//		FLinearColor Color;
//
//		switch (Grid[i])
//		{
//		case 1:
//			Color = FLinearColor::Black;
//			break;
//
//		case 2:
//			Color = FLinearColor::Red;
//			break;
//
//		case 3:
//			Color = FLinearColor::Green;
//			break;
//
//		default:
//			Color = FLinearColor::White;
//			break;
//		}
//
//		Cells[i]->SetBrushColor(Color);
//	}
//}

void UCPPW_WiresPuzzleBase::UpdateGridUI()
{
	for (int32 i = 0; i < 25; i++)
	{
		if (!Cells.IsValidIndex(i) || !Cells[i]) continue;

		FSlateBrush Brush;

		switch (Grid[i])
		{
		case 1: // pallina
			if (BallTexture)
			{
				Brush.SetResourceObject(BallTexture);
			}
			break;

		case 2: // buco
			if (HoleTexture)
			{
				Brush.SetResourceObject(HoleTexture);
			}
			break;

		case 3: // pallina nel buco
			if (BallInHoleTexture)
			{
				Brush.SetResourceObject(BallInHoleTexture);
			}
			break;

		default: // vuoto
			Brush.SetResourceObject(EmptyCellTexture);
			break;
		}

		Cells[i]->SetBrush(Brush);
	}
}

bool UCPPW_WiresPuzzleBase::CheckWin() const
{
	int32 CompletedTargets = 0;

	for (int32 i = 0; i < Grid.Num(); i++)
	{
		if (Grid[i] == 3)
		{
			CompletedTargets++;
		}
	}

	return CompletedTargets == 4;
}