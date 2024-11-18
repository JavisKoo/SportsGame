// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PurchaseManager.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct FCoinInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "CoinInfo")
	int infoCoinSeq;
	
	UPROPERTY(BlueprintReadOnly, Category = "CoinInfo")
	FString type;
	
	UPROPERTY(BlueprintReadOnly, Category = "CoinInfo")
	FString inAppId;
	
	UPROPERTY(BlueprintReadOnly, Category = "CoinInfo")
	int unit;
	
	UPROPERTY(BlueprintReadOnly, Category = "CoinInfo")
	int amt;
};


USTRUCT(BlueprintType, Blueprintable)
struct FCardPackInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "CardPackInfo")
	int32 infoSeq;

	UPROPERTY(BlueprintReadOnly, Category = "CardPackInfo")
	FString gachaPackId;

	UPROPERTY(BlueprintReadOnly, Category = "CardPackInfo")
	FString name;

	UPROPERTY(BlueprintReadOnly, Category = "CardPackInfo")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "CardPackInfo")
	int32 coin;
};

USTRUCT(BlueprintType, Blueprintable)
struct FCostumeInfoList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "CostumeList")
	FString ItemID;

	UPROPERTY(BlueprintReadOnly, Category = "CostumeLIst")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "CostumeList")
	int32 Coin;
};


UCLASS()
class RM_API UPurchaseManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPurchaseManager() {}
	~UPurchaseManager() {}

public:
	UFUNCTION(BlueprintCallable)
	FString ParsingJson(const FString& Value);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPurchaseGoogleFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UPurchaseAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPurchaseAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "PurchaseManager")
	FPurchaseGoogleFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "PurchaseManager")
	static UPurchaseAPI* PurchaseGoogle(const FString& receiptData);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "PurchaseManager")
	static UPurchaseAPI* GiftGoogle(const FString& receiptData, int32 TargetSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString receiptData;
	int32 TargetSeq;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPurchaseListFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FCoinInfo>&, Data);

UCLASS()
class RM_API UPurchaseListAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPurchaseListAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "PurchaseManager")
	FPurchaseListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "PurchaseManager")
	static UPurchaseListAPI* GetPurchaseList(const FString& ItemType);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	FString ItemType;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCardPackListFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FCardPackInfo>&, Data);

UCLASS()
class RM_API UCardPackListAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UCardPackListAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "PurchaseManager")
	FCardPackListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "PurchaseManager")
	static UCardPackListAPI* GetCardPackListInfo();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBuyGoldFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UBuyGoldAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UBuyGoldAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FBuyGoldFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UBuyGoldAPI* BuyGold(const FName& GoldKey);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString GoldKey;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCostumeListFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FCostumeInfoList>&, Data);

UCLASS()
class RM_API UCostumeListAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UCostumeListAPI() {}

	UPROPERTY(BlueprintAssignable, Category = "PurchaseManager")
	FCostumeListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "PurchaseManager")
	static UCostumeListAPI* GetCostumeList();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
};