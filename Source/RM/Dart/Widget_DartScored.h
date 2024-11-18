// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_DartScored.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UWidget_DartScored : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
		void SetScoreTextAndPlayAnim(int score);
	UFUNCTION(BlueprintCallable)
		void SetWidgetLocation(FVector WorldLocation);
protected:
	UFUNCTION()
		void AnimationFinished_BlendInScore();

private:
	UPROPERTY()
		class UTextBlock* scoreText;

	UPROPERTY(Meta = (BindWidgetAnim), Meta = (AllowPrivateAccess = true), Transient)
		UWidgetAnimation* BlendInScore;
	UPROPERTY()
		FWidgetAnimationDynamicEvent BlendInScore_EndDel;
};
