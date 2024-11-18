// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RM/RMStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "MiniGameSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMiniGameState : uint8
{
	None,
	
	Title,
	StartGame,
	ShootReady, Shoot, ShootFin,
	RoundEnd, NextRound,
	EndGame, ResumeGame, PausedGame,
	ExitGame,
};

UENUM(BlueprintType)
enum class EFreeKickTargetSize : uint8
{
	Big,
	Middle,
	Small,
};

UENUM(BlueprintType)
enum class EFreeKickTargetMoveType : uint8
{
	None,
	Horizontal,
	Vertical,
	Diagonal,
};

USTRUCT(Blueprintable)
struct FFreeKickRoundTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 라운드 정보")
	int Round;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 라운드 정보")
	int Stage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 라운드 정보")
	int Target_IDMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 라운드 정보")
	int Target_IDMax;
};

USTRUCT(Blueprintable)
struct FFreeKickTargetTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 목표물 정보")
	EFreeKickTargetSize TargetSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 목표물 정보")
	EFreeKickTargetMoveType MoveType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 목표물 정보")
	float MoveDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 목표물 정보")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=-2810, ClampMax=3440), Category = "프리킥 목표물 정보")
	float XLocationMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=-2810, ClampMax=3440), Category = "프리킥 목표물 정보")
	float XLocationMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=40, ClampMax=200), Category = "프리킥 목표물 정보")
	float YLocationMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=40, ClampMax=200), Category = "프리킥 목표물 정보")
	float YLocationMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "프리킥 목표물 정보")
	int Score;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedScore);

UCLASS()
class RM_API UMiniGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	/*
	virtual void Deinitialize() override;

	bool Tick(float DeltaSeconds);
	FDelegateHandle TickDelegateHandle;
	*/
	
public:
	UMiniGameSubsystem();

	UFUNCTION(BlueprintCallable, Category = "MiniGameSubsystem")
	void SetMiniGameType(EGameType GameType);

	UFUNCTION(BlueprintCallable, Category = "MiniGameSubsystem")
	void ChangeState(EMiniGameState miniGameState);

	UFUNCTION(BlueprintCallable, Category = "MiniGameSubsystem")
	void AddScore(int Score);

	UFUNCTION(BlueprintCallable, Category = "MiniGameSubsystem")
	int GetScore(int roundIndex = -1, int shootIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "MiniGameSubsystem")
	int GetRoundScore(int roundIndex = -1);
	
	UFUNCTION(BlueprintCallable, Category = "MiniGameSubsystem")
	int GetTotalScore();

	bool CheckFinalRound();

	UFUNCTION(BlueprintCallable, Category = "MiniGameSubsystem")
	FFreeKickTargetTable GetFreeKickTargetTable();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "MiniGameSubsystem")
	FOnChangedScore OnChangedScore;
	
//private:
	UPROPERTY(BlueprintReadOnly, Category="MiniGameSubsytem")
	EGameType GameType;

	UPROPERTY(BlueprintReadOnly, Category="MiniGameSubsytem")
	EMiniGameState MiniGameState;

	UPROPERTY(BlueprintReadOnly, Category="MiniGameSubsytem")
	int RoundIndex;

	UPROPERTY(BlueprintReadOnly, Category="MiniGameSubsytem")
	int RoundMax;

	UPROPERTY(BlueprintReadOnly, Category="MiniGameSubsytem")
	int StageIndex;

	UPROPERTY(BlueprintReadOnly, Category="MiniGameSubsytem")
	int StageMax;

private:
	TMap<int, TArray<int>> ScoresPerRound;
	UPROPERTY()
	UDataTable* DT_FreeKickRound = nullptr;
	UPROPERTY()
	UDataTable* DT_FreeKickTarget = nullptr;
};
