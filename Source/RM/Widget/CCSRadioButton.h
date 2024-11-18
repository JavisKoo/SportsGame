// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "CCSRadioButton.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UCCSRadioButton : public UButton
{
	GENERATED_UCLASS_BODY()
public:
	/** Sometimes a button should only be mouse-clickable and never keyboard focusable. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	int32 ButtonGroup = -1;


protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;


protected:
	/** Handle the actual click event from slate and forward it on */
	FReply SlateHandle2Clicked();
};
