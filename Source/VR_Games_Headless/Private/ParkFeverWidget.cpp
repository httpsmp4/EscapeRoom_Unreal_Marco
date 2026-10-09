#include "ParkFeverWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Engine/Texture2D.h"


// ============================================================
// CONSTRUCTOR
// ============================================================

UParkFeverWidget::UParkFeverWidget(
	const FObjectInitializer& ObjectInitializer
)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}


// ============================================================
// CONSTRUCT
// ============================================================

void UParkFeverWidget::NativeConstruct()
{
	Super::NativeConstruct();


	// ========================================================
	// BUTTON BINDING
	// ========================================================

	if (ColumnButton0)
	{
		ColumnButton0->OnClicked.AddUniqueDynamic(
			this,
			&UParkFeverWidget::HandleColumn0Clicked
		);
	}


	if (ColumnButton1)
	{
		ColumnButton1->OnClicked.AddUniqueDynamic(
			this,
			&UParkFeverWidget::HandleColumn1Clicked
		);
	}


	if (ColumnButton2)
	{
		ColumnButton2->OnClicked.AddUniqueDynamic(
			this,
			&UParkFeverWidget::HandleColumn2Clicked
		);
	}


	if (ColumnButton3)
	{
		ColumnButton3->OnClicked.AddUniqueDynamic(
			this,
			&UParkFeverWidget::HandleColumn3Clicked
		);
	}


	if (TutorialNextButton)
	{
		TutorialNextButton->OnClicked.AddUniqueDynamic(
			this,
			&UParkFeverWidget::HandleTutorialNextClicked
		);
	}


	if (InfoButton)
	{
		InfoButton->OnClicked.AddUniqueDynamic(
			this,
			&UParkFeverWidget::HandleInfoClicked
		);
	}


	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(
			this,
			&UParkFeverWidget::HandleRestartClicked
		);
	}


	SetupDemoLevel();

	OpenTutorial();
}


// ============================================================
// SETUP LEVEL
// ============================================================

void UParkFeverWidget::SetupDemoLevel()
{
	Columns.Empty();

	InventoryItems.Empty();

	TargetQueue.Empty();


	bGameWon = false;

	bGameOver = false;

	Moves = 0;


	// ========================================================
	// COLUMN 0
	// ========================================================

	{
		FEscapeItemColumn Column;

		Column.Items =
		{
			EEscapeItemType::Key,
			EEscapeItemType::Map,
			EEscapeItemType::Lantern,
			EEscapeItemType::Mask
		};

		Columns.Add(Column);
	}


	// ========================================================
	// COLUMN 1
	// ========================================================

	{
		FEscapeItemColumn Column;

		Column.Items =
		{
			EEscapeItemType::Lantern,
			EEscapeItemType::Mask,
			EEscapeItemType::Key,
			EEscapeItemType::Gear
		};

		Columns.Add(Column);
	}


	// ========================================================
	// COLUMN 2
	// ========================================================

	{
		FEscapeItemColumn Column;

		Column.Items =
		{
			EEscapeItemType::Map,
			EEscapeItemType::Gear,
			EEscapeItemType::Magnifier,
			EEscapeItemType::Key
		};

		Columns.Add(Column);
	}


	// ========================================================
	// COLUMN 3
	// ========================================================

	{
		FEscapeItemColumn Column;

		Column.Items =
		{
			EEscapeItemType::Gear,
			EEscapeItemType::Magnifier,
			EEscapeItemType::Mask,
			EEscapeItemType::Map
		};

		Columns.Add(Column);
	}


	// ========================================================
	// TARGET SEQUENCE
	// ========================================================

	TargetQueue =
	{
		EEscapeItemType::Mask,
		EEscapeItemType::Key,
		EEscapeItemType::Lantern,
		EEscapeItemType::Map,

		EEscapeItemType::Gear,
		EEscapeItemType::Magnifier,
		EEscapeItemType::Mask,
		EEscapeItemType::Key,

		EEscapeItemType::Map,
		EEscapeItemType::Gear,
		EEscapeItemType::Magnifier,
		EEscapeItemType::Mask,

		EEscapeItemType::Lantern,
		EEscapeItemType::Key,
		EEscapeItemType::Gear,
		EEscapeItemType::Map
	};


	RefreshUI();
}


// ============================================================
// RESTART
// ============================================================

void UParkFeverWidget::RestartGame()
{
	bTutorialOpen = false;

	TutorialPage = 0;

	SetupDemoLevel();

	RefreshUI();
}


// ============================================================
// SELECT COLUMN
// ============================================================

bool UParkFeverWidget::SelectColumn(
	int32 ColumnIndex
)
{
	// ========================================================
	// BLOCK INPUT
	// ========================================================

	if (
		bGameWon ||
		bGameOver ||
		bTutorialOpen
		)
	{
		return false;
	}


	// ========================================================
	// VALID COLUMN
	// ========================================================

	if (
		!Columns.IsValidIndex(
			ColumnIndex
		)
		)
	{
		return false;
	}


	if (
		Columns[
			ColumnIndex
		].Items.Num() == 0
				)
	{
		return false;
	}


	if (TargetQueue.Num() == 0)
	{
		CheckWinLose();

		RefreshUI();

		return false;
	}


	// ========================================================
	// SELECTED ITEM
	// ========================================================

	const EEscapeItemType SelectedItem =
		Columns[
			ColumnIndex
		].Items[0];


	const EEscapeItemType RequiredItem =
		TargetQueue[0];


	const bool bDirectMatch =
		SelectedItem ==
		RequiredItem;


	// ========================================================
	// INVENTORY FULL
	// ========================================================

	if (
		!bDirectMatch &&
		InventoryItems.Num() >=
		MaxInventoryItems
		)
	{
		BP_OnInvalidMove(
			ColumnIndex
		);


		CheckWinLose();

		RefreshUI();


		return false;
	}


	Moves++;


	// ========================================================
	// REMOVE FROM COLUMN
	// ========================================================

	Columns[
		ColumnIndex
	].Items.RemoveAt(0);


	// ========================================================
	// DIRECT MATCH
	// ========================================================

	if (bDirectMatch)
	{
		if (TargetQueue.Num() > 0)
		{
			TargetQueue.RemoveAt(0);
		}


		BP_OnValidMove(
			SelectedItem,
			ColumnIndex,
			true,
			0
		);
	}


	// ========================================================
	// INVENTORY
	// ========================================================

	else
	{
		const int32 DestinationIndex =
			InventoryItems.Num();


		InventoryItems.Add(
			SelectedItem
		);


		BP_OnValidMove(
			SelectedItem,
			ColumnIndex,
			false,
			DestinationIndex
		);
	}


	// ========================================================
	// AUTO-CONSUME
	// ========================================================

	AutoConsumeInventoryMatches();


	// ========================================================
	// CHECK RESULT
	// ========================================================

	CheckWinLose();


	RefreshUI();


	return true;
}


// ============================================================
// AUTO INVENTORY
// ============================================================

void UParkFeverWidget::AutoConsumeInventoryMatches()
{
	bool bFoundMatch =
		true;


	while (
		bFoundMatch &&
		TargetQueue.Num() > 0
		)
	{
		bFoundMatch =
			false;


		const EEscapeItemType RequiredItem =
			TargetQueue[0];


		for (
			int32 InventoryIndex = 0;
			InventoryIndex <
			InventoryItems.Num();
			++InventoryIndex
			)
		{
			if (
				InventoryItems[
					InventoryIndex
				] != RequiredItem
				)
			{
				continue;
			}


			const EEscapeItemType UsedItem =
				InventoryItems[
					InventoryIndex
				];


			InventoryItems.RemoveAt(
				InventoryIndex
			);


			TargetQueue.RemoveAt(0);


			BP_OnAutoInventoryUsed(
				UsedItem,
				InventoryIndex
			);


			bFoundMatch =
				true;


			break;
		}
	}
}


// ============================================================
// LOSS HELPERS
// ============================================================

bool UParkFeverWidget::IsRequiredItemInInventory() const
{
	if (TargetQueue.Num() == 0)
	{
		return false;
	}


	return InventoryItems.Contains(
		TargetQueue[0]
	);
}


bool UParkFeverWidget::IsRequiredItemOnAnyFront() const
{
	if (TargetQueue.Num() == 0)
	{
		return false;
	}


	const EEscapeItemType RequiredItem =
		TargetQueue[0];


	for (
		const FEscapeItemColumn& Column :
		Columns
		)
	{
		if (
			Column.Items.Num() > 0 &&
			Column.Items[0] ==
			RequiredItem
			)
		{
			return true;
		}
	}


	return false;
}


// ============================================================
// MINIMUM BLOCKERS
// ============================================================

int32 UParkFeverWidget::GetMinimumBlockersToRequiredItem() const
{
	if (TargetQueue.Num() == 0)
	{
		return 0;
	}


	const EEscapeItemType RequiredItem =
		TargetQueue[0];


	int32 MinimumBlockers =
		MAX_int32;


	bool bFoundItem =
		false;


	for (
		const FEscapeItemColumn& Column :
		Columns
		)
	{
		for (
			int32 ItemIndex = 0;
			ItemIndex <
			Column.Items.Num();
			++ItemIndex
			)
		{
			if (
				Column.Items[
					ItemIndex
				] ==
				RequiredItem
						)
			{
				MinimumBlockers =
					FMath::Min(
						MinimumBlockers,
						ItemIndex
					);


				bFoundItem =
					true;


				break;
			}
		}
	}


	if (!bFoundItem)
	{
		return -1;
	}


	return MinimumBlockers;
}


// ============================================================
// CAN REACH REQUIRED
// ============================================================

bool UParkFeverWidget::CanReachRequiredItem() const
{
	if (TargetQueue.Num() == 0)
	{
		return true;
	}


	if (IsRequiredItemInInventory())
	{
		return true;
	}


	if (IsRequiredItemOnAnyFront())
	{
		return true;
	}


	const int32 MinimumBlockers =
		GetMinimumBlockersToRequiredItem();


	if (MinimumBlockers < 0)
	{
		return false;
	}


	const int32 FreeInventorySlots =
		FMath::Max(
			0,
			MaxInventoryItems -
			InventoryItems.Num()
		);


	return
		MinimumBlockers <=
		FreeInventorySlots;
}


// ============================================================
// CHECK RESULT
// ============================================================

void UParkFeverWidget::CheckWinLose()
{
	if (
		bGameWon ||
		bGameOver
		)
	{
		return;
	}


	// ========================================================
	// WIN
	// ========================================================

	if (TargetQueue.Num() == 0)
	{
		bGameWon = true;

		bGameOver = false;


		OnGameWon.Broadcast();


		OnGameFinished.Broadcast(
			EEscapeGameResult::Won
		);


		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"ESCAPE PUZZLE: WON in %d moves"
			),
			Moves
		);


		return;
	}


	// ========================================================
	// LOSE
	// ========================================================

	if (!CanReachRequiredItem())
	{
		bGameOver = true;

		bGameWon = false;


		OnGameLost.Broadcast();


		OnGameFinished.Broadcast(
			EEscapeGameResult::Lost
		);


		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"ESCAPE PUZZLE: LOST"
			)
		);
	}
}


// ============================================================
// RESULT GETTERS
// ============================================================

bool UParkFeverWidget::IsGameFinished() const
{
	return
		bGameWon ||
		bGameOver;
}


bool UParkFeverWidget::DidPlayerWin() const
{
	return bGameWon;
}


EEscapeGameResult
UParkFeverWidget::GetGameResult() const
{
	if (bGameWon)
	{
		return EEscapeGameResult::Won;
	}


	if (bGameOver)
	{
		return EEscapeGameResult::Lost;
	}


	return EEscapeGameResult::InProgress;
}


int32 UParkFeverWidget::GetRemainingTargetCount() const
{
	return TargetQueue.Num();
}


int32 UParkFeverWidget::GetInventoryCount() const
{
	return InventoryItems.Num();
}


int32 UParkFeverWidget::GetMoveCount() const
{
	return Moves;
}


// ============================================================
// ITEM TEXTURE
// ============================================================

UTexture2D*
UParkFeverWidget::GetItemTextureForType(
	EEscapeItemType Item
) const
{
	switch (Item)
	{
	case EEscapeItemType::Key:
		return KeySprite.Get();


	case EEscapeItemType::Lantern:
		return LanternSprite.Get();


	case EEscapeItemType::Map:
		return MapSprite.Get();


	case EEscapeItemType::Magnifier:
		return MagnifierSprite.Get();


	case EEscapeItemType::Mask:
		return MaskSprite.Get();


	case EEscapeItemType::Gear:
		return GearSprite.Get();
	}


	return nullptr;
}


// ============================================================
// ITEM NAME
// ============================================================

FString UParkFeverWidget::GetItemDisplayName(
	EEscapeItemType Item
) const
{
	switch (Item)
	{
	case EEscapeItemType::Key:
		return TEXT("CHIAVE");


	case EEscapeItemType::Lantern:
		return TEXT("LANTERNA");


	case EEscapeItemType::Map:
		return TEXT("MAPPA");


	case EEscapeItemType::Magnifier:
		return TEXT("LENTE");


	case EEscapeItemType::Mask:
		return TEXT("MASCHERA");


	case EEscapeItemType::Gear:
		return TEXT("INGRANAGGIO");
	}


	return TEXT("?");
}


// ============================================================
// TUTORIAL
// ============================================================

void UParkFeverWidget::OpenTutorial()
{
	bTutorialOpen =
		true;


	TutorialPage =
		0;


	RefreshTutorial();

	RefreshButtons();
}


void UParkFeverWidget::CloseTutorial()
{
	bTutorialOpen =
		false;


	RefreshTutorial();

	RefreshButtons();
}


void UParkFeverWidget::NextTutorialPage()
{
	if (!bTutorialOpen)
	{
		return;
	}


	if (
		TutorialPage <
		TutorialPageCount - 1
		)
	{
		TutorialPage++;

		RefreshTutorial();

		return;
	}


	CloseTutorial();
}


// ============================================================
// REFRESH
// ============================================================

void UParkFeverWidget::RefreshUI()
{
	RefreshBoard();

	RefreshInventory();

	RefreshTargets();

	RefreshButtons();

	RefreshTutorial();

	RefreshResult();


	if (MovesText)
	{
		MovesText->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("MOSSE: %d"),
					Moves
				)
			)
		);
	}


	if (InventoryCountText)
	{
		InventoryCountText->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("%d / %d"),
					InventoryItems.Num(),
					MaxInventoryItems
				)
			)
		);
	}


	BP_OnUIRefreshed();
}


// ============================================================
// BOARD REFRESH
// ============================================================

void UParkFeverWidget::RefreshBoard()
{
	for (
		int32 ColumnIndex = 0;
		ColumnIndex < 4;
		++ColumnIndex
		)
	{
		for (
			int32 RowIndex = 0;
			RowIndex < 4;
			++RowIndex
			)
		{
			UImage* Image =
				GetBoardImage(
					ColumnIndex,
					RowIndex
				);


			if (!Image)
			{
				continue;
			}


			const bool bHasItem =
				Columns.IsValidIndex(
					ColumnIndex
				) &&
				Columns[
					ColumnIndex
				].Items.IsValidIndex(
					RowIndex
				);


					EEscapeItemType Item =
						EEscapeItemType::Key;


					if (bHasItem)
					{
						Item =
							Columns[
								ColumnIndex
							].Items[
								RowIndex
							];
					}


					SetImageItem(
						Image,
						bHasItem,
						Item
					);
		}
	}
}


// ============================================================
// INVENTORY REFRESH
// ============================================================

void UParkFeverWidget::RefreshInventory()
{
	for (
		int32 Index = 0;
		Index < 3;
		++Index
		)
	{
		UImage* Image =
			GetInventoryImage(
				Index
			);


		if (!Image)
		{
			continue;
		}


		const bool bHasItem =
			InventoryItems.IsValidIndex(
				Index
			);


		EEscapeItemType Item =
			EEscapeItemType::Key;


		if (bHasItem)
		{
			Item =
				InventoryItems[
					Index
				];
		}


		SetImageItem(
			Image,
			bHasItem,
			Item
		);
	}
}


// ============================================================
// TARGET REFRESH
// ============================================================

void UParkFeverWidget::RefreshTargets()
{
	for (
		int32 Index = 0;
		Index < 8;
		++Index
		)
	{
		UImage* Image =
			GetTargetImage(
				Index
			);


		if (!Image)
		{
			continue;
		}


		const bool bHasItem =
			Index <
			MaxVisibleTargets &&
			TargetQueue.IsValidIndex(
				Index
			);


		EEscapeItemType Item =
			EEscapeItemType::Key;


		if (bHasItem)
		{
			Item =
				TargetQueue[
					Index
				];
		}


		SetImageItem(
			Image,
			bHasItem,
			Item
		);
	}
}


// ============================================================
// BUTTON REFRESH
// ============================================================

void UParkFeverWidget::RefreshButtons()
{
	const bool bAllowInput =
		!bGameWon &&
		!bGameOver &&
		!bTutorialOpen;


	if (ColumnButton0)
	{
		ColumnButton0->SetIsEnabled(
			bAllowInput &&
			Columns.IsValidIndex(0) &&
			Columns[0].Items.Num() > 0
		);
	}


	if (ColumnButton1)
	{
		ColumnButton1->SetIsEnabled(
			bAllowInput &&
			Columns.IsValidIndex(1) &&
			Columns[1].Items.Num() > 0
		);
	}


	if (ColumnButton2)
	{
		ColumnButton2->SetIsEnabled(
			bAllowInput &&
			Columns.IsValidIndex(2) &&
			Columns[2].Items.Num() > 0
		);
	}


	if (ColumnButton3)
	{
		ColumnButton3->SetIsEnabled(
			bAllowInput &&
			Columns.IsValidIndex(3) &&
			Columns[3].Items.Num() > 0
		);
	}
}


// ============================================================
// TUTORIAL REFRESH
// ============================================================

void UParkFeverWidget::RefreshTutorial()
{
	if (TutorialRoot)
	{
		TutorialRoot->SetVisibility(
			bTutorialOpen
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed
		);
	}


	if (TutorialPage1)
	{
		TutorialPage1->SetVisibility(
			bTutorialOpen &&
			TutorialPage == 0
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed
		);
	}


	if (TutorialPage2)
	{
		TutorialPage2->SetVisibility(
			bTutorialOpen &&
			TutorialPage == 1
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed
		);
	}


	if (TutorialNextText)
	{
		TutorialNextText->SetText(
			FText::FromString(
				TutorialPage ==
				TutorialPageCount - 1
				? TEXT("GIOCA")
				: TEXT("AVANTI")
			)
		);
	}
}


// ============================================================
// RESULT REFRESH
// ============================================================

void UParkFeverWidget::RefreshResult()
{
	const bool bFinished =
		IsGameFinished();


	if (ResultRoot)
	{
		ResultRoot->SetVisibility(
			bFinished
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed
		);
	}


	if (
		ResultText &&
		bFinished
		)
	{
		ResultText->SetText(
			FText::FromString(
				bGameWon
				? TEXT("ENIGMA RISOLTO!")
				: TEXT("ENIGMA FALLITO")
			)
		);
	}
}


// ============================================================
// SET IMAGE
// ============================================================

void UParkFeverWidget::SetImageItem(
	UImage* Image,
	bool bHasItem,
	EEscapeItemType Item
)
{
	if (!Image)
	{
		return;
	}


	if (!bHasItem)
	{
		Image->SetVisibility(
			ESlateVisibility::Collapsed
		);

		return;
	}


	UTexture2D* Texture =
		GetItemTextureForType(
			Item
		);


	if (!Texture)
	{
		Image->SetVisibility(
			ESlateVisibility::Collapsed
		);

		return;
	}


	Image->SetBrushFromTexture(
		Texture,
		false
	);


	Image->SetVisibility(
		ESlateVisibility::HitTestInvisible
	);
}


// ============================================================
// GET BOARD IMAGE
// ============================================================

UImage* UParkFeverWidget::GetBoardImage(
	int32 ColumnIndex,
	int32 RowIndex
) const
{
	switch (ColumnIndex)
	{
	case 0:
	{
		switch (RowIndex)
		{
		case 0: return Item_C0_R0;
		case 1: return Item_C0_R1;
		case 2: return Item_C0_R2;
		case 3: return Item_C0_R3;
		}

		break;
	}


	case 1:
	{
		switch (RowIndex)
		{
		case 0: return Item_C1_R0;
		case 1: return Item_C1_R1;
		case 2: return Item_C1_R2;
		case 3: return Item_C1_R3;
		}

		break;
	}


	case 2:
	{
		switch (RowIndex)
		{
		case 0: return Item_C2_R0;
		case 1: return Item_C2_R1;
		case 2: return Item_C2_R2;
		case 3: return Item_C2_R3;
		}

		break;
	}


	case 3:
	{
		switch (RowIndex)
		{
		case 0: return Item_C3_R0;
		case 1: return Item_C3_R1;
		case 2: return Item_C3_R2;
		case 3: return Item_C3_R3;
		}

		break;
	}
	}


	return nullptr;
}


// ============================================================
// INVENTORY IMAGE
// ============================================================

UImage* UParkFeverWidget::GetInventoryImage(
	int32 Index
) const
{
	switch (Index)
	{
	case 0:
		return InventoryItem0;

	case 1:
		return InventoryItem1;

	case 2:
		return InventoryItem2;
	}


	return nullptr;
}


// ============================================================
// TARGET IMAGE
// ============================================================

UImage* UParkFeverWidget::GetTargetImage(
	int32 Index
) const
{
	switch (Index)
	{
	case 0:
		return TargetItem0;

	case 1:
		return TargetItem1;

	case 2:
		return TargetItem2;

	case 3:
		return TargetItem3;

	case 4:
		return TargetItem4;

	case 5:
		return TargetItem5;

	case 6:
		return TargetItem6;

	case 7:
		return TargetItem7;
	}


	return nullptr;
}


// ============================================================
// BUTTON HANDLERS
// ============================================================

void UParkFeverWidget::HandleColumn0Clicked()
{
	SelectColumn(0);
}


void UParkFeverWidget::HandleColumn1Clicked()
{
	SelectColumn(1);
}


void UParkFeverWidget::HandleColumn2Clicked()
{
	SelectColumn(2);
}


void UParkFeverWidget::HandleColumn3Clicked()
{
	SelectColumn(3);
}


void UParkFeverWidget::HandleTutorialNextClicked()
{
	NextTutorialPage();
}


void UParkFeverWidget::HandleInfoClicked()
{
	OpenTutorial();
}


void UParkFeverWidget::HandleRestartClicked()
{
	RestartGame();
}