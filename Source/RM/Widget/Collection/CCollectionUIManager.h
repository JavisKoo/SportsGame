#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UCollectionStructs.h"
#include "CCollectionUIManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateCollectionUI);

UCLASS()
class RM_API ACCollectionUIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ACCollectionUIManager();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTable")
		UDataTable* DataTablePlayerInfoList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTable")
		UDataTable* DataTablePlayerPieceList;

public:
	UFUNCTION(BlueprintCallable)
		FCCollectionPlayerInfo GetPlayerInfo(FName RowName);

	UFUNCTION(BlueprintCallable)
		int32 GetPlayerPiece(FName RowName);

	UFUNCTION(BlueprintCallable)
		FName SetSelcetedPlayer() { return SelectedPlayerName; }

	UFUNCTION(BlueprintCallable)
		void GetAllPlayerNameArray();

	UFUNCTION(BlueprintCallable)
		void GetPlayersCard(FName &Player, TArray<FName> &RestOfPlayers);

	UFUNCTION(BlueprintCallable)
		int32 GetMaxPieces() { return MaxPiece; }

private:
	void GetRandomPiece(FName& Player, TArray<FName>& RestOfPlayers);
	void GetAcheivePiece(FName& Player, TArray<FName>& RestOfPlayers);
	void SetWidgetCard();
	void MakeCards();
public:
	UFUNCTION(BlueprintCallable)
		TArray<class UCUserWidget_PlayerCard*> GetCards();

	int32 GetMaxCardIndex();
private:
	void CollectionUIManagerValueSetting();

protected:
	virtual void BeginPlay() override;

public:
	//UPROPERTY(BlueprintAssignable)
	FUpdateCollectionUI OnUpdateCollectionUI;

	UFUNCTION(BlueprintCallable)
		void CallUpdateCollectionUI();

private:
	//UI에 넘겨줄 선수리스트
	TArray<FName> PlayerNameArray;
	TArray<FName> AllPlayerNameArray;

	//지정된 선수가 있을 시 재정의
	UPROPERTY(EditDefaultsOnly)
		FName SelectedPlayerName;

	//실제콜렉션에 영향을 받는 최대조각수 정의
	FCCollectionUIManagerSetting ValueSetting;
	int32 MaxPiece;

	//실제콜렉션에 영향을 받지 않고 UI상에서만 다뤄짐
	UPROPERTY(EditDefaultsOnly)
		int32 MaxCardIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UCUserWidget_PlayerCard> WidgetCardClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TArray<class UCUserWidget_PlayerCard*> CardsArray;
};
