#pragma once

#include "CoreMinimal.h"
#include "PlayerUserData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FPlayerUserData
{
	GENERATED_BODY()

public:
	FPlayerUserData()
	: PlayerPieceCount(0), bAcquisition(false), bInPlayer(false)
	{}

	FPlayerUserData(const int32 InPlayerPieceCount, const bool bInAcquisition, const bool bInInPlayer)
	: PlayerPieceCount(InPlayerPieceCount), bAcquisition(bInAcquisition), bInPlayer(bInInPlayer)
	{}

	UPROPERTY(BlueprintReadWrite, Category = PlayerUserData)
	int32 PlayerPieceCount;

	UPROPERTY(BlueprintReadWrite, Category = PlayerUserData)
	bool bAcquisition;

	UPROPERTY(BlueprintReadWrite, Category = PlayerUserData)
	bool bInPlayer;
};
