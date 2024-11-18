// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TaskCommand.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,Blueprintable)
class UTaskCommand : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RM_API ITaskCommand
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISpawnPlayer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void IRand(ERandType RandType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetGender(EGenderRM Gender);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetHeadScale(float Scale);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetHandScale(float Scale);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetScale(float Scale);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetTopSize(float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetBottomSize(float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetCheekSize(float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetHead(FName RowName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetMorpherTargetValue(EMorphTarget Target, float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetEyeMaterialScalarParameter(EEyeMaterialScalarParameter Enum,float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetCostumePart(ECostumePart Part, FName RowName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
	void ISetSkinColor(FLinearColor SkinColor);

//	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Costume")
//	void ISetEyeColor(float HUEValue);


};
