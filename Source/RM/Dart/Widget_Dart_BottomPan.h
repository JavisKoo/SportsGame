// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Dart_BottomPan.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UWidget_Dart_BottomPan : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
		void SetScoreTextBoxes(TArray<int> scoreValues);
	UFUNCTION(BlueprintCallable)
		void SetScoreTextBoxInIndex(int index, int scoreValue);
	UFUNCTION(BlueprintCallable)
		void SetLeftScoreText(int lastRoundIndex, int leftScore);
	UFUNCTION(BlueprintCallable)
		void ResetScoreTextBoxes(int DefualtLeftScore);
public:
	UPROPERTY(VisibleAnywhere)
		class UHorizontalBox* HorizontalBox_Round;
	UPROPERTY(VisibleAnywhere)
		class UHorizontalBox* HorizontalBox_Score;

};
