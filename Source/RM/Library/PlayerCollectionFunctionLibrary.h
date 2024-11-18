// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RM/Data/PlayerUserData.h"
#include "PlayerCollectionFunctionLibrary.generated.h"

class UDataTable;

UCLASS(BlueprintType)
class RM_API UPlayerCollectionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = PlayerCollectionFunctionLibrary)
	static const FPlayerUserData GetPlayerUserData(const int32 PlayerId);

	UFUNCTION(BlueprintCallable, Category = PlayerCollectionFunctionLibrary)
	static int32 GetAcquirablePlayerId();

private:
	static const int32 MaxAcquirablePlayerPieceCount;

	static TArray<FName> PlayerDataRowNames;
	static int32 AcquirablePlayerRowNameIndex;
};
