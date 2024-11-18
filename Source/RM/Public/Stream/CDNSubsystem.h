// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CDNSubsystem.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FCDNErrorDelegate, int32, ErrorCode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCDNResponseDelegate, FString, Response);

DECLARE_DYNAMIC_DELEGATE_OneParam(FPOSTErrorDelegate, int32, ErrorCode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPOSTResponseDelegate, FString, Response);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefreshDelegate);

USTRUCT(Blueprintable)
struct FCDN
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 Seq;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VideoUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ThumbnailUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TeamNameLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TeamNameRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CountLike;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsSelected;

};

USTRUCT(Blueprintable)
struct Flinks
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Current;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Last;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString First;
};


USTRUCT(Blueprintable)
struct FPage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RequestApi;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentPage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PrevPage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Total;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FirstPage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LastPage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	Flinks links;
};

USTRUCT(Blueprintable)
struct FDATA
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FPage Pagination;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FCDN> list;
	
	void FromJson(const FString& JsonString);
	void FromObject(TSharedPtr<FJsonObject> JsonObject);
};

USTRUCT(Blueprintable)
struct FVideoLikeResponse
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 CountLike;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool IsSelected;
	
	void FromObject(TSharedPtr<FJsonObject> JsonObject);
};


USTRUCT(Blueprintable)
struct FVideoLikes
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 Seq;
	
	FString ToJson();

};



UCLASS()
class RM_API UCDNSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void GetCDN(const FCDNErrorDelegate& InError, const FCDNResponseDelegate& InResponse, FString Category, const FString LanguageCode, FString Page, FString Limit);

	UFUNCTION(BlueprintCallable)
	void PostLikes(const FPOSTErrorDelegate& InError, const FPOSTResponseDelegate& InResponse, int32 Seq);

	UFUNCTION(BlueprintCallable)
	void Refresh();
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FDATA CDNData;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVideoLikes VideoLikes;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVideoLikeResponse VideoLikeResponse;

	
	FCDNErrorDelegate   OnError;
	FCDNResponseDelegate OnResponse;
	FPOSTErrorDelegate   OnErrorPost;
	FPOSTResponseDelegate OnResponsePost;
	
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FRefreshDelegate OnRefresh;
};
