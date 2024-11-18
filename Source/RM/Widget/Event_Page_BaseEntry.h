// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvanceWebBrowser.h"
#include "Blueprint/UserWidget.h"
#include "Event_Page_BaseEntry.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetCallBackFromJS, FString, Param);

UCLASS()
class RM_API UEvent_Page_BaseEntry : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable)
	FOnGetCallBackFromJS OnGetCallBackFromJS;

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void LoadURL(FString URL, FString BindObjectName, UObject* Object = nullptr, bool bIsPermenent = true);

	UFUNCTION(BlueprintCallable)
	void LoadURLButtonWeb(FString URL, FString BindObjectName, UObject* Object = nullptr, bool bIsPermenent = true);
private:
	UPROPERTY()
	UAdvanceWebBrowser* AdvanceWebBrowser;
	UPROPERTY()
	UAdvanceWebBrowser* AdvanceWebBrowserWithButton;

	UPROPERTY()
	UObject* BindObject;
	
};
