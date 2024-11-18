#include "CUserWidget_PlayerBookCollection.h"
#include "Kismet/GameplayStatics.h"
#include "CUserWidget_CollectionPlayerCard.h"
#include "CCollectionUIManager.h"
#include "Components/Button.h"

void UCUserWidget_PlayerBookCollection::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_CollectionDetail->OnClicked.AddDynamic(this, &UCUserWidget_PlayerBookCollection::OnClick_CollectionDetail);
	Button_Exit->OnClicked.AddDynamic(this, &UCUserWidget_PlayerBookCollection::OnClick_Exit);

	CollectionManager = Cast<ACCollectionUIManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACCollectionUIManager::StaticClass()));

	if (!UKismetSystemLibrary::IsValid(CollectionManager))
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, "NativeOnInitialized");

	CollectionManager->OnUpdateCollectionUI.AddDynamic(this, &UCUserWidget_PlayerBookCollection::UpdateCollectionPlayersCard);
	CollectionPlayerCard->SetMaxPieces(CollectionManager->GetMaxPieces());
	CollectionPlayerCard->SetMaxCards(CollectionManager->GetMaxCardIndex());
	CollectionPlayerCard->SetCollectionManager(CollectionManager);

}

void UCUserWidget_PlayerBookCollection::OnClick_CollectionDetail()
{
	//도감정보 수집
}

void UCUserWidget_PlayerBookCollection::OnClick_Exit()
{
	SetVisibility(ESlateVisibility::Hidden);
}


void UCUserWidget_PlayerBookCollection::GetSelectedPlayer()
{
	if (!UKismetSystemLibrary::IsValid(CollectionManager))
	{
		return;
	}

	CollectionManager->GetPlayersCard(SelectedPlayer, RestOfPlayers);
}

void UCUserWidget_PlayerBookCollection::UpdateCollectionPlayersCard()
{
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Blue, "UpdateCollectionPlayersCard");

	if (!UKismetSystemLibrary::IsValid(CollectionManager))
	{
		return;
	}

	TArray<FCCollectionPlayerInfo> UnAcheiveCardsArray;

	FName SelectedPlayerValue;
	TArray<FName> RestOfPlayersValue;
	CollectionManager->GetPlayersCard(SelectedPlayerValue, RestOfPlayersValue);

	for (FName ROP : RestOfPlayersValue)
	{
		UnAcheiveCardsArray.Add(CollectionManager->GetPlayerInfo(ROP));
	}

	FCCollectionPlayerInfo sPlayerCard = CollectionManager->GetPlayerInfo(SelectedPlayerValue);
	CollectionPlayerCard->UpdateCollectionPlayersCard(sPlayerCard, UnAcheiveCardsArray);
	UpdateCollectionPlayerNameAndPieces(SelectedPlayerValue);
	UpdateCollectionAcheivePlayerImage(sPlayerCard);
}

void UCUserWidget_PlayerBookCollection::UpdateCollectionAcheivePlayerImage(FCCollectionPlayerInfo AcheiveCard)
{
	CollectionPlayerCard->UpdateCollectionAcheivePlayerImage(AcheiveCard);
}

void UCUserWidget_PlayerBookCollection::UpdateCollectionPlayerNameAndPieces(FName PlayerName)
{
	if (!UKismetSystemLibrary::IsValid(CollectionManager))
	{
		return;
	}

	CollectionPlayerCard->UpdatePieceCount(CollectionManager->GetPlayerPiece(PlayerName));
	CollectionPlayerCard->UpdatePieceName(CollectionManager->GetPlayerInfo(PlayerName));
}
