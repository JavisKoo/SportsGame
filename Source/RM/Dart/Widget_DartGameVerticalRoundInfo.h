// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_DartGameVerticalRoundInfo.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UWidget_DartGameVerticalRoundInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
		void ResetAllRoundScore_VB();
	UFUNCTION(BlueprintCallable)
		void SetAllRoundScore_VB(const TArray<int>& scores);
	UFUNCTION(BlueprintCallable)
		void SetRoundScoreByIndex_VB(const int index, const int value);
	UFUNCTION(BlueprintCallable)
		void SetScoreLeftText(const int lastRoundIndex, const int leftScore);
private:
	UPROPERTY()
		class UVerticalBox* VB_RoundScoreBox;

};
