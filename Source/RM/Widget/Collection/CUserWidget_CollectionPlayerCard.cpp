#include "CUserWidget_CollectionPlayerCard.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/GridPanel.h"
#include "CCollectionUIManager.h"
#include "CUserWidget_PlayerCard.h"

void UCUserWidget_CollectionPlayerCard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Material Path
	//FString mPath = "MaterialInstanceConstant'/Game/Dev/CollectionUI/Resource/Material/MI_CollectionGrayScale.MI_CollectionGrayScale'";
	//mMat = Cast<UMaterialInstanceConstant>(StaticLoadObject(UMaterialInstanceConstant::StaticClass(), NULL, *mPath));
}

void UCUserWidget_CollectionPlayerCard::UpdateCollectionPlayersCard(FCCollectionPlayerInfo AcheiveCard, TArray<FCCollectionPlayerInfo> UnAcheiveCards)
{
	int32 AcheiveIndex = 0;
	int32 UnAcheiveIndex = 0;
	TArray<FCCollectionPlayerInfo> CardArray;
	
	CardArray = UnAcheiveCards;
	CardArray.Add(AcheiveCard);
	
	//SetAcheiveIndex
	AcheiveIndex = UKismetMathLibrary::RandomInteger(MaxCard);

	AddCardsToGridPanel();

	//카드슬롯 수 만큼 카드를 표시합니다. 선택된 카드 와 선택되지않은 카드 표시
	for (int32 CardIndex = 0; CardIndex < MaxCard; ++CardIndex)
	{
		if (AcheiveIndex == CardIndex)
		{
			UpdateIndexCard(CardIndex, AcheiveIndex, AcheiveCard);
		}
		else
		{
			UpdateIndexCard(CardIndex, AcheiveIndex, CardArray[UnAcheiveIndex]);
	
			UnAcheiveIndex += 1;
	
			if (UnAcheiveIndex >= CardArray.Num())
			{
				UnAcheiveIndex = 0;
			}
		}
	}
}

void UCUserWidget_CollectionPlayerCard::UpdateCollectionAcheivePlayerImage(FCCollectionPlayerInfo AcheiveCard)
{
	Image_SelectedPlayer->SetBrushFromTexture(Cast<UTexture2D>(AcheiveCard.Image));
}

void UCUserWidget_CollectionPlayerCard::UpdatePieceName(FCCollectionPlayerInfo CollectionPlayerInfo)
{
	TextBlock_Piece_Name_Text->SetText(FText::FromString(CollectionPlayerInfo.Name));
}

void UCUserWidget_CollectionPlayerCard::UpdatePieceCount(int32 Piece)
{
	TArray<FStringFormatArg> PieceStringFormatArgs = { Piece, MaxPiece };
	FString PieceText = FString::Format(TEXT("{0} To {1}"), PieceStringFormatArgs);
	TextBlock_Piece_Image_Text->SetText(FText::FromString(PieceText));
}

void UCUserWidget_CollectionPlayerCard::SetMaxPieces(int32 MaxPieces)
{
	MaxPiece = MaxPieces;
}

void UCUserWidget_CollectionPlayerCard::SetMaxCards(int32 MaxCards)
{
	MaxCard = MaxCards;
}

void UCUserWidget_CollectionPlayerCard::SetCollectionManager(ACCollectionUIManager* CManager)
{
	CollectionManager = CManager;
}

void UCUserWidget_CollectionPlayerCard::UpdateIndexCard(int32 CardIndex, int32 AcheiveIndex, FCCollectionPlayerInfo CollectionPlayerInfo)
{
	if (CardIndex >= PlayerCards.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "Index Overflow");
		return;
	}

	if (!UKismetSystemLibrary::IsValid(PlayerCards[CardIndex]))
	{
		return;
	}

	if (CardIndex == AcheiveIndex)
	{
		PlayerCards[CardIndex]->UpdateCard(CollectionPlayerInfo, true);
	}
	else
	{
		PlayerCards[CardIndex]->UpdateCard(CollectionPlayerInfo, false);
	}
}

void UCUserWidget_CollectionPlayerCard::AddCardsToGridPanel()
{
	GridPanel_Cards->ClearChildren();

	if (!UKismetSystemLibrary::IsValid(CollectionManager))
	{
		return;
	}

	PlayerCards = CollectionManager->GetCards();

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::FromInt(PlayerCards.Num()));

	//카드슬롯에 GrayScaleMaterial Setting, 슬롯배열을 GridPanel에 추가
	for (int32 CardIndex = 0; CardIndex < PlayerCards.Num(); ++CardIndex)
	{
		PlayerCards[CardIndex]->SetCardMaterial(UMaterialInstanceDynamic::Create(mMat, this));
		GridPanel_Cards->AddChildToGrid(PlayerCards[CardIndex], 0, CardIndex);
	}
}
