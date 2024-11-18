// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PaperSprite.h"
#include "SpriteFunctions.generated.h"

/**
 * 
 */
UCLASS()
class RM_API USpriteFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Source Size", Keywords = "Source Texture Sprite"), Category = Custom) //Here you can change the keywords, name and category
		static FVector2D GetSourceSize(UPaperSprite* sprite);
};