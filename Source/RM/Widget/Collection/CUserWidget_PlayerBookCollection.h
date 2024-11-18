#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCollectionStructs.h"
#include "CUserWidget_PlayerBookCollection.generated.h"

UCLASS()
class RM_API UCUserWidget_PlayerBookCollection : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void UpdateCollectionPlayersCard();

private:
	void GetSelectedPlayer();
	void UpdateCollectionAcheivePlayerImage(FCCollectionPlayerInfo AcheiveCard);
	void UpdateCollectionPlayerNameAndPieces(FName PlayerName);

public:
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_CollectionDetail;

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Exit;

	UPROPERTY(meta = (BindWidget))
		class UCUserWidget_CollectionPlayerCard* CollectionPlayerCard;

private:
	UFUNCTION()
		void OnClick_CollectionDetail();

	UFUNCTION()
		void OnClick_Exit();

protected:
	void NativeOnInitialized()override;

private:
	FName SelectedPlayer;
	TArray<FName> RestOfPlayers;
private:
	class ACCollectionUIManager* CollectionManager;
};
