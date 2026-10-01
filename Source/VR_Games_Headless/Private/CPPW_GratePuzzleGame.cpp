#include "CPPW_GratePuzzleGame.h"

void UCPPW_GratePuzzleGame::NativeConstruct()
{
	Super::NativeConstruct();

	Cells.Empty();

	Cells.Add(Cell_0_0); // 0
	Cells.Add(Cell_0_1); // 1
	Cells.Add(Cell_0_2); // 2

	Cells.Add(Cell_1_0); // 3
	Cells.Add(Cell_1_1); // 4
	Cells.Add(Cell_1_2); // 5

	SolutionSequence.Empty();

	SolutionSequence.Add(4);
	SolutionSequence.Add(5);
	SolutionSequence.Add(2);
	SolutionSequence.Add(1);
	SolutionSequence.Add(4);
	SolutionSequence.Add(3);
	SolutionSequence.Add(0);
	SolutionSequence.Add(1);
	SolutionSequence.Add(4);
	SolutionSequence.Add(5);
	SolutionSequence.Add(2);
	SolutionSequence.Add(1);
	SolutionSequence.Add(4);
	SolutionSequence.Add(3);
	SolutionSequence.Add(4);

	ResetPuzzle();
}

void UCPPW_GratePuzzleGame::ResetPuzzle()
{
	CurrentStep = 0;
	bPuzzleCompleted = false;

	UpdateGridUI();
}

void UCPPW_GratePuzzleGame::OnCellClicked(int32 Index)
{
	if (bPuzzleCompleted)
	{
		return;
	}

	if (!Cells.IsValidIndex(Index))
	{
		return;
	}

	if (!SolutionSequence.IsValidIndex(CurrentStep))
	{
		return;
	}

	const int32 ExpectedIndex = SolutionSequence[CurrentStep];

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Clicked Cell: %d | Expected: %d | Step: %d"),
		Index,
		ExpectedIndex,
		CurrentStep
	);

	if (Index == ExpectedIndex)
	{
		CurrentStep++;

		if (IsPuzzleSolved())
		{
			bPuzzleCompleted = true;

			UE_LOG(LogTemp, Warning, TEXT("GRATE PUZZLE COMPLETED"));

			UpdateGridUI();
			OnPuzzleCompleted();
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong cell. Resetting sequence."));
		CurrentStep = 0;
	}

	UpdateGridUI();
}

bool UCPPW_GratePuzzleGame::IsPuzzleSolved() const
{
	return CurrentStep >= SolutionSequence.Num();
}

void UCPPW_GratePuzzleGame::UpdateGridUI()
{
	for (int32 i = 0; i < Cells.Num(); i++)
	{
		if (!Cells.IsValidIndex(i) || !Cells[i]) continue;

		ApplyCellVisual(i);
	}
}

void UCPPW_GratePuzzleGame::ApplyCellVisual(int32 Index)
{
	if (!Cells.IsValidIndex(Index) || !Cells[Index]) return;

	FLinearColor Color = NormalColor;

	if (bPuzzleCompleted)
	{
		Color = CompletedColor;
	}
	else
	{
		// Evidenzia le celle già cliccate nella sequenza
		for (int32 Step = 0; Step < CurrentStep; Step++)
		{
			if (SolutionSequence.IsValidIndex(Step) && SolutionSequence[Step] == Index)
			{
				Color = FLinearColor(0.25f, 0.65f, 1.0f, 1.0f); // già usata
			}
		}

		// Evidenzia la prossima cella da cliccare
		if (SolutionSequence.IsValidIndex(CurrentStep) && SolutionSequence[CurrentStep] == Index)
		{
			Color = ActiveColor;
		}
	}

	SetCellColor(Cells[Index], Color);
}

void UCPPW_GratePuzzleGame::SetCellColor(UBorder* Cell, const FLinearColor& Color)
{
	if (!Cell) return;

	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Box;

	Cell->SetBrush(Brush);
	Cell->SetBrushColor(Color);
}