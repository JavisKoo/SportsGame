// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TaskCommand.h"
#include "Task.generated.h"

UCLASS()
class RM_API ATask : public AActor, public ITaskCommand
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATask();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
