// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "QuestSubsystem.h"
#include "QuestSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UQuestSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestSaveGame")
	TArray<FQuestState> Daily;
};
