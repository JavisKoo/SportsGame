// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_DartBoard.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UWidget_DartBoard : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
		void SetScoreBoxText(int index, int score);
	UFUNCTION(BlueprintCallable)
		void SetScoreLeftText(int leftScore);
	UFUNCTION(BlueprintCallable)
		int GetLeftScore();
	UFUNCTION(BlueprintCallable)
		void ResetAllScoreBoxText();

public:

	////////////////////////////////////////////////////////////
	////			New				//	
	////////////////////////////////////////////////////////////
	UPROPERTY()
		class UTextBlock* Score_01Text;
	UPROPERTY()
		class UTextBlock* Score_02Text;
	UPROPERTY()
		class UTextBlock* Score_03Text;
	UPROPERTY()
		class UTextBlock* Score_leftText;
	UPROPERTY()
		TArray<class UImage*> IMG_DartImages;
};
