// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RM/RMStruct.h"
#include "CardGameAPIManager.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UCardGameAPIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
};


// 보유중인 카드 조회
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FGetMyCardListFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FCardListElem>&, Data, const float&, TrophyBoost);

UCLASS()
class RM_API UGetMyCardList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetMyCardList() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FGetMyCardListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UGetMyCardList* GetMyCardList();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};


// 카드팩 뽑기 요청
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostGachaCardPackkFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const FGachaResult&, GachaResult);

UCLASS()
class RM_API UPostGachaCardPack : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostGachaCardPack() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FPostGachaCardPackkFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UPostGachaCardPack* PostGachaCardPack(FString gachaPackID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString gachaPackID;
};

// 카드팩 리스트 조회 (뽑기 목록)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetCardPackListFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FCardPackList>&, Data);

UCLASS()
class RM_API UGetCardPackList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetCardPackList() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FGetCardPackListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UGetCardPackList* GetCardPackList();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

// 카드 업그레이드 요청
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostCardUpgradeFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const int&, resultCode);

UCLASS()
class RM_API UPostCardUpgrade : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostCardUpgrade() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FPostCardUpgradeFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UPostCardUpgrade* PostCardUpgrade(FString cardUID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString cardUID;
};
//tutorialupgrade
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostCardUpgradetutorialFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const int&, resultCode);

UCLASS()
class RM_API UPostCardUpgradetutorial : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostCardUpgradetutorial() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
	FPostCardUpgradetutorialFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
	static UPostCardUpgradetutorial* PostCardUpgradetutorial(FString cardUID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString cardUID;
};

// 게임 결과 전송 함수
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostCardGameResultFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const FCardGameResult&, ResultReturn);

UCLASS()
class RM_API UPostCardGameResult : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostCardGameResult() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FPostCardGameResultFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UPostCardGameResult* PostCardGameResult(int32 enemyUserSeq, FString gameRoomName, int32 gameResult, int32 gamePoint);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 enemyUserSeq;
	FString gameRoomName;
	/* 0: lose, 1: win */
	int32 gameResult;
	int32 gamePoint;
};

// 랭킹 요청 함수
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FPostCardGameRankFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FCardRankElem>&, RankArray, const int32&, myTrophy);

UCLASS()
class RM_API UPostCardGameRank : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostCardGameRank() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FPostCardGameRankFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UPostCardGameRank* PostCardGameRank(int32 rankType);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 rankType;
};

//골드 차감 요청 함수
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPostDeductGoldFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UPostDeductGold : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostDeductGold() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FPostDeductGoldFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UPostDeductGold* PostDeductGold(int32 GamePoint ,FString GamePointType);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString GamePointType;
	int32 GamePoint;
};

// NFT카드 업그레이드 요청
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostNFTCardUpgradeFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const int&, resultCode);

UCLASS()
class RM_API UPostNFTCardUpgrade : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostNFTCardUpgrade() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FPostNFTCardUpgradeFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UPostNFTCardUpgrade* PostNFTCardUpgrade(FString cardUID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString cardUID;
};

// NFT카드 등급 업그레이드 요청
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostNFTCardGradeUpgradeFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const int&, resultCode);

UCLASS()
class RM_API UPostNFTCardGradeUpgrade : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostNFTCardGradeUpgrade() {};

	UPROPERTY(BlueprintAssignable, Category = "CardGameAPI")
		FPostNFTCardGradeUpgradeFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "CardGameAPI")
		static UPostNFTCardGradeUpgrade* PostNFTCardGradeUpgrade(FString cardUID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString cardUID;
};