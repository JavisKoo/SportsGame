#pragma once

#include "CoreMinimal.h"
#include "PlayerUserData.h"
#include "NetworkData.generated.h"

struct FPlayerUserData;

UENUM(BlueprintType)
enum class ENetworkStatus : uint8
{
	None,
	Success,
	Failed
};

USTRUCT(BlueprintType)
struct FNetworkData
{
	GENERATED_BODY()

public:
	FString Message;
};

USTRUCT(BlueprintType)
struct FResponseData : public FNetworkData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = ResponseData)
	ENetworkStatus Status = ENetworkStatus::None;
};

USTRUCT(BlueprintType)
struct FPlayerUserResponseData : public FResponseData
{
	GENERATED_BODY()

public:
	FPlayerUserResponseData()
	: PlayerId(INDEX_NONE)
	{}

	FPlayerUserResponseData(const FPlayerUserData& InPlayerUserData, const int32 InPlayerId)
	: PlayerUserData(InPlayerUserData), PlayerId(InPlayerId)
	{}

	UPROPERTY(BlueprintReadOnly, Category = ResponseData)
	FPlayerUserData PlayerUserData;
	
	UPROPERTY(BlueprintReadOnly, Category = ResponseData)
	int32 PlayerId;
};
