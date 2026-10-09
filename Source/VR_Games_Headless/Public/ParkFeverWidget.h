#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParkFeverWidget.generated.h"

class UTexture2D;
class UImage;
class UButton;
class UTextBlock;
class UWidget;


// ============================================================
// ITEM TYPES
// ============================================================

UENUM(BlueprintType)
enum class EEscapeItemType : uint8
{
	Key			UMETA(DisplayName = "Chiave"),
	Lantern		UMETA(DisplayName = "Lanterna"),
	Map			UMETA(DisplayName = "Mappa"),
	Magnifier	UMETA(DisplayName = "Lente"),
	Mask		UMETA(DisplayName = "Maschera"),
	Gear		UMETA(DisplayName = "Ingranaggio")
};


// ============================================================
// GAME RESULT
// ============================================================

UENUM(BlueprintType)
enum class EEscapeGameResult : uint8
{
	InProgress	UMETA(DisplayName = "In Progress"),
	Won			UMETA(DisplayName = "Won"),
	Lost		UMETA(DisplayName = "Lost")
};


// ============================================================
// COLUMN
//
// Items[0] = primo oggetto / cliccabile.
// In BP puoi mettere R0 graficamente IN BASSO.
// ============================================================

USTRUCT(BlueprintType)
struct FEscapeItemColumn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EEscapeItemType> Items;
};


// ============================================================
// EVENTS
// ============================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FEscapeSimpleEvent
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FEscapeGameFinishedEvent,
	EEscapeGameResult,
	Result
);


// ============================================================
// WIDGET
// ============================================================

UCLASS()
class VR_GAMES_HEADLESS_API UParkFeverWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UParkFeverWidget(
		const FObjectInitializer& ObjectInitializer
	);


	// ========================================================
	// CONFIG
	// ========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Game"
	)
	int32 MaxInventoryItems = 3;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Game"
	)
	int32 MaxVisibleTargets = 8;


	// ========================================================
	// ITEM SPRITES
	//
	// Questi sono gli UNICI sprite dinamici gestiti dal C++.
	//
	// Background, celle, frame, inventario ecc.
	// li metti direttamente nel Widget Blueprint.
	// ========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Sprites"
	)
	TObjectPtr<UTexture2D> KeySprite = nullptr;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Sprites"
	)
	TObjectPtr<UTexture2D> LanternSprite = nullptr;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Sprites"
	)
	TObjectPtr<UTexture2D> MapSprite = nullptr;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Sprites"
	)
	TObjectPtr<UTexture2D> MagnifierSprite = nullptr;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Sprites"
	)
	TObjectPtr<UTexture2D> MaskSprite = nullptr;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Escape Puzzle|Sprites"
	)
	TObjectPtr<UTexture2D> GearSprite = nullptr;


	// ========================================================
	// GAME STATE
	// ========================================================

	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Game"
	)
	TArray<FEscapeItemColumn> Columns;


	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Game"
	)
	TArray<EEscapeItemType> InventoryItems;


	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Game"
	)
	TArray<EEscapeItemType> TargetQueue;


	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Game"
	)
	bool bGameWon = false;


	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Game"
	)
	bool bGameOver = false;


	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Game"
	)
	int32 Moves = 0;


	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Tutorial"
	)
	bool bTutorialOpen = false;


	UPROPERTY(
		BlueprintReadOnly,
		Category = "Escape Puzzle|Tutorial"
	)
	int32 TutorialPage = 0;


	// ========================================================
	// EVENTS
	// ========================================================

	UPROPERTY(
		BlueprintAssignable,
		Category = "Escape Puzzle|Events"
	)
	FEscapeSimpleEvent OnGameWon;


	UPROPERTY(
		BlueprintAssignable,
		Category = "Escape Puzzle|Events"
	)
	FEscapeSimpleEvent OnGameLost;


	UPROPERTY(
		BlueprintAssignable,
		Category = "Escape Puzzle|Events"
	)
	FEscapeGameFinishedEvent OnGameFinished;


	// ========================================================
	// GAME FUNCTIONS
	// ========================================================

	UFUNCTION(
		BlueprintCallable,
		Category = "Escape Puzzle"
	)
	void SetupDemoLevel();


	UFUNCTION(
		BlueprintCallable,
		Category = "Escape Puzzle"
	)
	void RestartGame();


	UFUNCTION(
		BlueprintCallable,
		Category = "Escape Puzzle"
	)
	bool SelectColumn(
		int32 ColumnIndex
	);


	UFUNCTION(
		BlueprintCallable,
		Category = "Escape Puzzle"
	)
	void RefreshUI();


	// ========================================================
	// TUTORIAL
	// ========================================================

	UFUNCTION(
		BlueprintCallable,
		Category = "Escape Puzzle|Tutorial"
	)
	void OpenTutorial();


	UFUNCTION(
		BlueprintCallable,
		Category = "Escape Puzzle|Tutorial"
	)
	void CloseTutorial();


	UFUNCTION(
		BlueprintCallable,
		Category = "Escape Puzzle|Tutorial"
	)
	void NextTutorialPage();


	// ========================================================
	// RESULT
	// ========================================================

	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle"
	)
	bool IsGameFinished() const;


	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle"
	)
	bool DidPlayerWin() const;


	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle"
	)
	EEscapeGameResult GetGameResult() const;


	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle"
	)
	int32 GetRemainingTargetCount() const;


	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle"
	)
	int32 GetInventoryCount() const;


	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle"
	)
	int32 GetMoveCount() const;


	// ========================================================
	// UTILITY PER BLUEPRINT
	// ========================================================

	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle|Sprites"
	)
	UTexture2D* GetItemTextureForType(
		EEscapeItemType Item
	) const;


	UFUNCTION(
		BlueprintPure,
		Category = "Escape Puzzle"
	)
	FString GetItemDisplayName(
		EEscapeItemType Item
	) const;


	// ========================================================
	// BLUEPRINT VISUAL EVENTS
	//
	// Questi servono SOLO per animazioni.
	//
	// La logica rimane C++.
	// ========================================================

	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Escape Puzzle|Visual Events"
	)
	void BP_OnValidMove(
		EEscapeItemType Item,
		int32 ColumnIndex,
		bool bDirectMatch,
		int32 DestinationIndex
	);


	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Escape Puzzle|Visual Events"
	)
	void BP_OnInvalidMove(
		int32 ColumnIndex
	);


	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Escape Puzzle|Visual Events"
	)
	void BP_OnAutoInventoryUsed(
		EEscapeItemType Item,
		int32 PreviousInventoryIndex
	);


	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Escape Puzzle|Visual Events"
	)
	void BP_OnUIRefreshed();


protected:

	virtual void NativeConstruct() override;


private:

	static constexpr int32 TutorialPageCount = 2;


	// ========================================================
	// BOARD IMAGES
	//
	// IMPORTANTE:
	// R0 = primo/cliccabile.
	// Puoi posizionarlo graficamente in fondo alla colonna.
	// ========================================================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C0_R0 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C0_R1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C0_R2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C0_R3 = nullptr;


	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C1_R0 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C1_R1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C1_R2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C1_R3 = nullptr;


	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C2_R0 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C2_R1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C2_R2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C2_R3 = nullptr;


	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C3_R0 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C3_R1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C3_R2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Item_C3_R3 = nullptr;


	// ========================================================
	// COLUMN BUTTONS
	// ========================================================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ColumnButton0 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ColumnButton1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ColumnButton2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ColumnButton3 = nullptr;


	// ========================================================
	// INVENTORY
	// ========================================================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> InventoryItem0 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> InventoryItem1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> InventoryItem2 = nullptr;


	// ========================================================
	// TARGETS
	// ========================================================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem0 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem3 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem4 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem5 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem6 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TargetItem7 = nullptr;


	// ========================================================
	// TUTORIAL
	// ========================================================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> TutorialRoot = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> TutorialPage1 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> TutorialPage2 = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> TutorialNextButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TutorialNextText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> InfoButton = nullptr;


	// ========================================================
	// RESULT
	// ========================================================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ResultRoot = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> RestartButton = nullptr;


	// ========================================================
	// EXTRA INFO
	// ========================================================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MovesText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InventoryCountText = nullptr;


	// ========================================================
	// BUTTON HANDLERS
	// ========================================================

	UFUNCTION()
	void HandleColumn0Clicked();

	UFUNCTION()
	void HandleColumn1Clicked();

	UFUNCTION()
	void HandleColumn2Clicked();

	UFUNCTION()
	void HandleColumn3Clicked();

	UFUNCTION()
	void HandleTutorialNextClicked();

	UFUNCTION()
	void HandleInfoClicked();

	UFUNCTION()
	void HandleRestartClicked();


	// ========================================================
	// LOGIC
	// ========================================================

	void AutoConsumeInventoryMatches();

	void CheckWinLose();

	bool IsRequiredItemInInventory() const;

	bool IsRequiredItemOnAnyFront() const;

	int32 GetMinimumBlockersToRequiredItem() const;

	bool CanReachRequiredItem() const;


	// ========================================================
	// UI INTERNAL
	// ========================================================

	UImage* GetBoardImage(
		int32 ColumnIndex,
		int32 RowIndex
	) const;

	UImage* GetInventoryImage(
		int32 Index
	) const;

	UImage* GetTargetImage(
		int32 Index
	) const;

	void SetImageItem(
		UImage* Image,
		bool bHasItem,
		EEscapeItemType Item
	);

	void RefreshBoard();

	void RefreshInventory();

	void RefreshTargets();

	void RefreshButtons();

	void RefreshTutorial();

	void RefreshResult();
};