// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "RM/MiniGame/MiniGameSubsystem.h"
#include "RM/RM_Singleton.h"
#include "CPP_Freekick_Left_Pan.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UCPP_Freekick_Left_Pan : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
		void RefreshWidget();
	
private:
	UPROPERTY(meta=(bindWidget))
		UVerticalBox* VB_Round;

	UPROPERTY(meta=(bindWidget))
		UVerticalBox* VB_Score;
};
