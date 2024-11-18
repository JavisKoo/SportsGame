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
	//UI�� �Ѱ��� ��������Ʈ
	TArray<FName> PlayerNameArray;
	TArray<FName> AllPlayerNameArray;

	//������ ������ ���� �� ������
	UPROPERTY(EditDefaultsOnly)
		FName SelectedPlayerName;

	//�����ݷ��ǿ� ������ �޴� �ִ������� ����
	FCCollectionUIManagerSetting ValueSetting;
	int32 MaxPiece;

	//�����ݷ��ǿ� ������ ���� �ʰ� UI�󿡼��� �ٷ���
	UPROPERTY(EditDefaultsOnly)
		int32 MaxCardIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UCUserWidget_PlayerCard> WidgetCardClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TArray<class UCUserWidget_PlayerCard*> CardsArray;
};
