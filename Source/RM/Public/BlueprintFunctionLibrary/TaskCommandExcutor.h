// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RMDefinedEnum.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TaskCommandExcutor.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UTaskCommandExcutor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SpawnPlayer")
	static void SpawnPlayer(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Rand")
	static void Rand(const UObject* WorldContextObject, ERandType RandType);

	/*
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RandMorphTargetValue")
	static void RandMorphTargetValue(const UObject* WorldContextObject,EMorphGroup Group);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RandHead")
	static void RandHead(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RandFaceSkin")
	static void RandFaceSkin(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RandSkinColor")
	static void RandSkinColor(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RandHandScale")
	static void RandHandScale(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RandScale")
	static void RandScale(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RandHeadScale")
	static void RandHeadScale(const UObject* WorldContextObject);
	*/

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SetGender")
	static void SetGender(const UObject* WorldContextObject, EGenderRM Gender);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SetScale")
	static void SetScale(const UObject* WorldContextObject, float Scale);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SetScale")
	static void SetSkinColor(const UObject* WorldContextObject, FLinearColor SkinColor);


	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SetHeadScale")
	static void SetHeadScale(const UObject* WorldContextObject, float Scale);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SetHandScale")
	static void SetHandScale(const UObject* WorldContextObject,float Scale);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SetTopSize")
	static void SetTopSize(const UObject* WorldContextObject, float Size);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "CheekSize")
	static void SetCheekSize(const UObject* WorldContextObject, float Size);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "BottomSize")
	static void SetBottomSize(const UObject* WorldContextObject, float Size);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Head")
	static void SetCostumePart(const UObject* WorldContextObject,ECostumePart Part, FName RowName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Morpher")
	static void SetMorperTargetValue(const UObject* WorldContextObject, EMorphTarget Target, float Value);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "CheekSize")
	static void SetEyeMaterialScalarParameter(const UObject* WorldContextObject, EEyeMaterialScalarParameter Enum, float Value);


};
