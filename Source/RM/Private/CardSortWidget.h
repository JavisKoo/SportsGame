// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <algorithm>
#include "Engine/UserDefinedEnum.h"
#include "CardSortWidget.generated.h"

/**
 * 
 */
UCLASS()
class UCardSortWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	//TArray<FString> Names;
	//TArray<int32> Powers;
	//TArray<FString>Position;

private:
	UFUNCTION(BlueprintCallable)
		void SortByName(const TMap<int32, FString>& InputMap, const bool isDescending, TMap<int32, FString>& OutMAp);

	UFUNCTION(BlueprintCallable)
		void SortByPower(const TMap<int32, int32>& InputMap, const bool isDescending, TMap<int32, int32>& OutMap);

	UFUNCTION(BlueprintCallable)
		void SortByPosition();
};

UENUM(BlueprintType)
enum class SortState : uint8
{
	AtoZ,
	Power,
	Grade
};