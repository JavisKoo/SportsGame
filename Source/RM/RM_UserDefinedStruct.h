// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "RM_UserDefinedStruct.generated.h"

/**
 * 
 */
UCLASS()
class RM_API URM_UserDefinedStruct : public UUserDefinedStruct
{
	GENERATED_BODY()
	
};

USTRUCT(Blueprintable, BlueprintType)
struct FTimeInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite)
	FDateTime AccountEndTime;
};
