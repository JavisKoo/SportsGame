// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Event_Daily_Entry_Base.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UEvent_Daily_Entry_Base : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void InitEntry(const FText& rowName, const int& GroupInterval);
	virtual void InitEntry_Implementation(const FText& rowName, const int& GroupInterval) {};

protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetItemBrush();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Widget Sequence", ExposeOnSpawn = true))
		int WidgetSeq;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "TicketCount", ExposeOnSpawn = true))
		int TicketCnt;
};
