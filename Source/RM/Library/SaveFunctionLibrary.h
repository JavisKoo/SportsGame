// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RM/Data/PlayerUserData.h"
#include "SaveFunctionLibrary.generated.h"

UCLASS()
class RM_API USaveFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = SaveFunctionLibrary)
	static void SavePlayerUserDataJson(const int32 PlayerPieceCount, const bool bAcquisition, const bool bInPlayer, const FString& JsonPath);

	UFUNCTION(BlueprintCallable, Category = SaveFunctionLibrary)
	static FPlayerUserData GetPlayerUserDataByJson(const FString& JsonPath);
};
