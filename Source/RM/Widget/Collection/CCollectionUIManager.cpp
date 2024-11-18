#include "CCollectionUIManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CUserWidget_PlayerCard.h"


ACCollectionUIManager::ACCollectionUIManager()
{
	//위젯 생성전에 위젯클라스를 저장
	SetWidgetCard();
}

FCCollectionPlayerInfo ACCollectionUIManager::GetPlayerInfo(FName RowName)
{
	// DataTablePlayerInfoList 가 NULL일때 처리해야함
	FCCollectionPlayerInfo Info;

	if (!UKismetSystemLibrary::IsValid(DataTablePlayerInfoList))
	{
		return Info;
	}

	FCCollectionPlayerInfo* Item = DataTablePlayerInfoList->FindRow<FCCollectionPlayerInfo>(RowName, "");
	if (!Item)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0F, FColor::Yellow, "Wrong RowName in DataTablePlayerInfoList");
		return Info;
	}
	return *Item;
}

int32 ACCollectionUIManager::GetPlayerPiece(FName RowName)
{
	// DataTablePlayerPieceList 가 NULL일때 MaxPiece넘겨줌
	// MaxPiece는 BeginPlay()때 초기화됨
	if (!UKismetSystemLibrary::IsValid(DataTablePlayerPieceList))
	{
		return MaxPiece;
	}

	FCCollectionPlayerPiece* Item = DataTablePlayerPieceList->FindRow<FCCollectionPlayerPiece>(RowName, "");
	if (!Item)
	{
		return MaxPiece;
	}
	return Item->Piece;
}

void ACCollectionUIManager::GetAllPlayerNameArray()
{
	TArray<FName> RowNames = DataTablePlayerPieceList->GetRowNames();
	for (int32 TableRowName = 0; TableRowName < RowNames.Num(); ++TableRowName)
	{
		AllPlayerNameArray.AddUnique(RowNames[TableRowName]);
	}
}

void ACCollectionUIManager::GetPlayersCard(FName& Player, TArray<FName>& RestOfPlayers)
{
	//NULL일때 return값 처리
	if (!UKismetSystemLibrary::IsValid(DataTablePlayerPieceList))
	{
		return;
	}

	TArray<FName> RowNames = DataTablePlayerPieceList->GetRowNames();

	bool bSelectedPlayerName = false;
	for (FName RowName : RowNames)
	{
		FCCollectionPlayerPiece* Item = DataTablePlayerPieceList->FindRow<FCCollectionPlayerPiece>(RowName, "");

		if (Item->Piece < MaxPiece)
		{
			PlayerNameArray.AddUnique(RowName);
		}

		//데이타테이블에 없는 RowName은 공백으로 처리
		if (SelectedPlayerName.IsEqual(RowName))
		{
			bSelectedPlayerName = true;
		}
	}

	if (PlayerNameArray.Num() > 0)
	{
		//FString.IsEmpty 랑 동일하게 작동 되는지
		if (SelectedPlayerName.IsNone() || !bSelectedPlayerName)
		{
			GetRandomPiece(Player, RestOfPlayers);
		}
		else
		{
			GetAcheivePiece(Player, RestOfPlayers);
		}
	}
	else
	{
		GetAllPlayerNameArray();
		FString UnSelectedPlayer;
		TArray<FName> UnSelectedPlayerArray;

		UnSelectedPlayerArray = AllPlayerNameArray;
		if (SelectedPlayerName.IsNone() || !bSelectedPlayerName)
		{
			Player = UnSelectedPlayerArray[0];
			UnSelectedPlayerArray.Remove(Player);
			RestOfPlayers = UnSelectedPlayerArray;
		}
		else
		{
			UnSelectedPlayerArray.Remove(Player);
			RestOfPlayers = UnSelectedPlayerArray;
		}
	}
}

void ACCollectionUIManager::GetRandomPiece(FName& Player, TArray<FName>& RestOfPlayers)
{
	FName SelectedPlayer;
	TArray<FName> CopyPlayerNameArray = PlayerNameArray;

	if (CopyPlayerNameArray.Num() > 0)
	{
		SelectedPlayer = CopyPlayerNameArray[UKismetMathLibrary::RandomInteger(CopyPlayerNameArray.Num())];
		CopyPlayerNameArray.Remove(SelectedPlayer);
		Player = SelectedPlayer;
		RestOfPlayers = CopyPlayerNameArray;
	}
	else
	{
		Player = FName("Player0");
		RestOfPlayers = PlayerNameArray;
	}
}

void ACCollectionUIManager::GetAcheivePiece(FName& Player, TArray<FName>& RestOfPlayers)
{
	FName SelectedPlayer;
	TArray<FName> CopyPlayerNameArray = PlayerNameArray;

	if (PlayerNameArray.Num() > 0)
	{
		SelectedPlayer = SelectedPlayerName;
		CopyPlayerNameArray.Remove(SelectedPlayer);
		Player = SelectedPlayer;
		RestOfPlayers = CopyPlayerNameArray;
	}
	else
	{
		Player = FName("Player0");
		RestOfPlayers = PlayerNameArray;
	}
}

void ACCollectionUIManager::SetWidgetCard()
{
	//FString fPath = "WidgetBlueprint'/Game/Dev/CollectionUI/Collection/WB_PlayerCard.WB_PlayerCard_C'";
	//ConstructorHelpers::FClassFinder<UCUserWidget_PlayerCard> asset(*fPath);
	//WidgetCardClass = asset.Class;
}

void ACCollectionUIManager::MakeCards()
{
	if (!UKismetSystemLibrary::IsValid(WidgetCardClass))
	{
		return;
	}

	//UI에 공개되는 카드목적의 슬롯 수
	for (int32 IndexCard = 0; IndexCard < MaxCardIndex; IndexCard++)
	{
		CardsArray.Add(CreateWidget<UCUserWidget_PlayerCard>(GetWorld(), WidgetCardClass));
	}
}

TArray<class UCUserWidget_PlayerCard*> ACCollectionUIManager::GetCards()
{
	return CardsArray;
}

int32 ACCollectionUIManager::GetMaxCardIndex()
{
	return MaxCardIndex;
}

void ACCollectionUIManager::CollectionUIManagerValueSetting()
{
	MaxPiece = ValueSetting.MaxPieces;
}

void ACCollectionUIManager::BeginPlay()
{
	Super::BeginPlay();
	CollectionUIManagerValueSetting();
	GetAllPlayerNameArray();
	MakeCards();
}

void ACCollectionUIManager::CallUpdateCollectionUI()
{
	if (OnUpdateCollectionUI.IsBound())
	{
		OnUpdateCollectionUI.Broadcast();
	}
}
