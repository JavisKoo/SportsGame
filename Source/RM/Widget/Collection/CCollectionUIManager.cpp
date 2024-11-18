#include "CCollectionUIManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CUserWidget_PlayerCard.h"


ACCollectionUIManager::ACCollectionUIManager()
{
	//���� �������� ����Ŭ�󽺸� ����
	SetWidgetCard();
}

FCCollectionPlayerInfo ACCollectionUIManager::GetPlayerInfo(FName RowName)
{
	// DataTablePlayerInfoList �� NULL�϶� ó���ؾ���
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
	// DataTablePlayerPieceList �� NULL�϶� MaxPiece�Ѱ���
	// MaxPiece�� BeginPlay()�� �ʱ�ȭ��
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
	//NULL�϶� return�� ó��
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

		//����Ÿ���̺� ���� RowName�� �������� ó��
		if (SelectedPlayerName.IsEqual(RowName))
		{
			bSelectedPlayerName = true;
		}
	}

	if (PlayerNameArray.Num() > 0)
	{
		//FString.IsEmpty �� �����ϰ� �۵� �Ǵ���
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

	//UI�� �����Ǵ� ī������� ���� ��
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
