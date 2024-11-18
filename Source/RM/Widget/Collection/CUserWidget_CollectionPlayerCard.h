#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCollectionStructs.h"
#include "CUserWidget_CollectionPlayerCard.generated.h"

UCLASS()
class RM_API UCUserWidget_CollectionPlayerCard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void UpdateCollectionPlayersCard(FCCollectionPlayerInfo AcheiveCard, TArray<FCCollectionPlayerInfo> UnAcheiveCards);

	UFUNCTION(BlueprintCallable)
		void UpdateCollectionAcheivePlayerImage(FCCollectionPlayerInfo AcheiveCard);

	UFUNCTION(BlueprintCallable)
		void UpdatePieceName(FCCollectionPlayerInfo CollectionPlayerInfo);

	UFUNCTION(BlueprintCallable)
		void UpdatePieceCount(int32 Piece);

	UFUNCTION(BlueprintCallable)
		void SetMaxPieces(int32 MaxPieces);

	void SetMaxCards(int32 MaxCards);
public:
	void SetCollectionManager(class ACCollectionUIManager* CManager);

private:
	void UpdateIndexCard(int32 CardIndex,int32 AcheiveIndex, FCCollectionPlayerInfo CollectionPlayerInfo);
	void AddCardsToGridPanel();

public:
	UPROPERTY(meta = (BindWidget))
		class UGridPanel* GridPanel_Cards;

	UPROPERTY(meta = (BindWidget))
		class UImage* Image_SelectedPlayer;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Piece_Name_Text;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Piece_Image_Text;

private:
	int32 MaxPiece;
	int32 MaxCard;
protected:
	void NativeOnInitialized()override;
private:
	class ACCollectionUIManager* CollectionManager;
private:
	TArray<class UCUserWidget_PlayerCard*> PlayerCards;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		class UMaterialInstanceConstant* mMat;
};
