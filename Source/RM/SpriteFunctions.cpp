// Fill out your copyright notice in the Description page of Project Settings.

#include "SpriteFunctions.h"
#include "RM.h"
// #include "PaperSprite.h"

FVector2D USpriteFunctions::GetSourceSize(UPaperSprite* sprite)
{
	if (sprite == nullptr)
		return FVector2D(0, 0);
	
	return sprite->GetSlateAtlasData().GetSourceDimensions();
}

