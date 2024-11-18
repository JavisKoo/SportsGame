// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonobjectConverter.h"
#include "Misc/DateTime.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchInfoSubsystem.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FMatchInfoErrorDelegate, int32, ErrorCode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMatchInfoResponseDelegate, FString, Response);

// Soccer Match Struct
USTRUCT(Blueprintable, BlueprintType)
struct FStatistics
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Statistics")
		int32 win;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Statistics")
		int32 draw;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Statistics")
		int32 lose;
};
USTRUCT(Blueprintable, BlueprintType)
struct FHome
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Home")
	int32 seq;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Home")
	FString code;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Home")
	int32 LastScore;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Home")
	int32 psoScore;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Home")
	FString name;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Home")
	FString country;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Home")
	FString iconImgURL;
};

USTRUCT(Blueprintable, BlueprintType)
struct FAway
{
	GENERATED_USTRUCT_BODY()
public:
	
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Away")
	int32 seq;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Away")
	FString code;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Away")
	int32 LastScore;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Away")
	int32 psoScore;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Away")
	FString name;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Away")
	FString country;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Away")
	FString iconImgURL;
	
};
USTRUCT(Blueprintable, BlueprintType)
struct FMatchesAT
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "matchesAT")
	FString year;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "matchesAT")
	FString date;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "matchesAT")
	FString time;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "matchesAT")
	int64 timestamp;

};
USTRUCT(Blueprintable, BlueprintType)
struct Fvote
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Vote")
	int32 home;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Vote")
	int32 draw;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Vote")
	int32 away;

	
};
USTRUCT(Blueprintable, BlueprintType)
struct FLeague
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "League")
	int32 id;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "League")
	int32 year;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "League")
	FString name;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "League")
	FString season;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "League")
	FString code;
};

USTRUCT(Blueprintable, BlueprintType)
struct FlatestMatch
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	int32 id;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	FString match;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	FHome home;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	FAway away;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	FMatchesAT matchesAT;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	Fvote vote;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	FLeague league;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "latestMatch")
	FString status;
};

USTRUCT(Blueprintable, BlueprintType)
struct FHeadtoHead
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "HeadtoHead")
	FStatistics statistics;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "HeadtoHead")
	FlatestMatch latestMatch;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPast
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Past")
	TArray<FlatestMatch> List;
};
USTRUCT(Blueprintable, BlueprintType)
struct FMatchInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Matchinfo")
	FHeadtoHead headtohead;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Matchinfo")
	FPast past;

	void FromJson(const FString& JsonString);
	void FromObject(TSharedPtr<FJsonObject> JsonObject);
};


UCLASS()
class RM_API UMatchInfoSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void GetMatchInfo(const FMatchInfoErrorDelegate& InError, const FMatchInfoResponseDelegate& InResponse, FString Clubcode);

	UFUNCTION(BlueprintCallable)
	void ConvertTimeStampToDateTime(int64 TimeStamp, FString& OutDate, FString& OutTime, FDateTime& OutDateTime);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMatchInfo MatchInfo;

	FMatchInfoErrorDelegate OnError;
	FMatchInfoResponseDelegate OnResponse;
	
	
};
