/*
*	
*/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "RM/RMProtocolFunctionLibrary.h"
#include "RMNFTSubsystem.generated.h"

UENUM(BlueprintType)
enum class EBiddingState : uint8
{
	EProgress,
	Done
};



DECLARE_DYNAMIC_DELEGATE_OneParam(FNFTErrorDelegate, int32, ErrorCode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FNFTResponeDelegate, FString, Respone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateNFTList);

USTRUCT(Blueprintable)
struct FNFT
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Seq;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OpenSeaID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TokenID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AssetContractAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImageURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImageIngameURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImagePreviewUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImageThumbnailUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImageOriginalurl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Permalink;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OwnerAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OwnerUserName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CreatorAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumSales;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastSale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TopBid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TopOfferOffererAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TopOfferOrderHash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TopOfferStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TopOfferEndTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TopOfferPrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TopOfferSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CreatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UpdatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinimumBid;

	void FromJson(const FString& JsonString);
	void FromObject(TSharedPtr<FJsonObject> JsonObject);
};

UCLASS()
class RM_API URMNFTSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RequestNFTList(FString thumbnail, const FOnHttpRequestResponseDelegate& InResponse, const FOnHttpRequestErrorDelegate& InError);
	
	UFUNCTION(BlueprintCallable)
	void GetNFT(const FNFTErrorDelegate& InError, const FNFTResponeDelegate& InRespone, FString thumbnail);

	UFUNCTION(BlueprintCallable)
	FString GetNFTURL();

private:
	bool SetNFTListData(const TSharedPtr<FJsonObject> Data);

private:
	UPROPERTY()
	FOnHttpRequestResponseDelegate OnRequestNftInfoFinished;

	UPROPERTY()
	FOnHttpRequestErrorDelegate OnRequestNftInfoError;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateNFTList OnUpdateNftList;

public:
	UPROPERTY(BlueprintReadOnly, Category = "NFTSubSystem")
	TMap<int, FNFT> NFTListInfoTable;


protected:
	TSharedPtr<FJsonObject> Parsing(FHttpResponsePtr pResponse, int32& OutResponeCode);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FNFT>		NFT;

	FNFTErrorDelegate   OnError;
	FNFTResponeDelegate OnRespone;
};
