// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameSubsystem.h"



UMiniGameSubsystem::UMiniGameSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> RoundTableObject(TEXT("/Game/RM/DataTables/DT_FreeKickRoundTable.DT_FreeKickRoundTable"));
	if (RoundTableObject.Succeeded())
		DT_FreeKickRound = RoundTableObject.Object;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> TargetTableObject(TEXT("/Game/RM/DataTables/DT_FreeKickTargetTable.DT_FreeKickTargetTable"));
	if (TargetTableObject.Succeeded())
		DT_FreeKickTarget = TargetTableObject.Object;
}

void UMiniGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

/*
void UMiniGameSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UMiniGameSubsystem::Tick(float DeltaSeconds)
{
	switch (MiniGameState)
	{
	// case EMiniGameState::None: break;
	// case EMiniGameState::Title: break;
	// case EMiniGameState::StartGame: break;
	case EMiniGameState::ShootReady:
		break;
	// case EMiniGameState::Shoot: break;
	// case EMiniGameState::ShootFin: break;
	// case EMiniGameState::RoundEnd: break;
	// case EMiniGameState::EndGame: break;
	// case EMiniGameState::ResumeGame: break;
	// case EMiniGameState::PausedGame: break;
	// case EMiniGameState::ExitGame: break;
	// default: ;
	}
	
	return true;
}
*/

void UMiniGameSubsystem::SetMiniGameType(EGameType gameType)
{
	GameType = gameType;
}

void UMiniGameSubsystem::ChangeState(EMiniGameState miniGameState)
{
	if (MiniGameState != miniGameState)
	{
		MiniGameState = miniGameState;
		switch (miniGameState)
		{
			case EMiniGameState::None: break;
			// case Init
			case EMiniGameState::Title:
				// if (TickDelegateHandle.IsValid() == false)
				// 	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UMiniGameSubsystem::Tick));

				RoundMax = 3;
				StageMax = 3;

				for (int i = 0; i < RoundMax; i++)
					ScoresPerRound.Add(i, TArray<int>());
			
				break;
			case EMiniGameState::StartGame:
				switch (GameType)
				{
					case EGameType::BallTrapping: break;
					case EGameType::Dart_501: break;
								
					case EGameType::FreeKick:
						RoundIndex = 0;
						StageIndex = 0;

						for (int i = 0; i < RoundMax; i++)
							ScoresPerRound[i] = TArray<int>();
						break;
					default:
						UE_LOG(LogTemp, Log, TEXT("올바른 게임 타입을 설정해주세요."));
				}
				break;
			case EMiniGameState::ShootReady: break;
			case EMiniGameState::Shoot: break;
			case EMiniGameState::ShootFin: break;
			case EMiniGameState::RoundEnd: break;
			case EMiniGameState::NextRound:
				StageIndex = 0;
				RoundIndex++;
				break;
			case EMiniGameState::EndGame:
				break;
			case EMiniGameState::ResumeGame: break;
			case EMiniGameState::PausedGame: break;
			case EMiniGameState::ExitGame:
				// if (TickDelegateHandle.IsValid())
				// 	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
				break;
			default: ;
		}
	}
}

void UMiniGameSubsystem::AddScore(int Score)
{
	if (ScoresPerRound.Contains(RoundIndex) && StageIndex < StageMax)
	{
		StageIndex++;
		ScoresPerRound[RoundIndex].Add(Score);

		if (OnChangedScore.IsBound())
			OnChangedScore.Broadcast();
	}
}

int UMiniGameSubsystem::GetScore(int roundIndex, int shootIndex)
{
	if (roundIndex < 0) roundIndex = RoundIndex;
	if (shootIndex < 0) shootIndex = StageIndex;

	if (ScoresPerRound.Contains(roundIndex) && shootIndex < ScoresPerRound[roundIndex].Num())
		return ScoresPerRound[roundIndex][shootIndex];
	return 0;
}

int UMiniGameSubsystem::GetRoundScore(int roundIndex)
{
	if (roundIndex < 0)
		roundIndex = RoundIndex;

	int RoundScore = 0;
	if (ScoresPerRound.Contains(roundIndex))
	{
		for (const auto& Iterator : ScoresPerRound[roundIndex])
			RoundScore += Iterator;
	}
	
	return RoundScore;
}

int UMiniGameSubsystem::GetTotalScore()
{
	int TotalScore = 0;
	for (const auto& Iterator : ScoresPerRound)
		TotalScore += GetRoundScore(Iterator.Key);
	return TotalScore;
}

bool UMiniGameSubsystem::CheckFinalRound()
{
	return RoundIndex == (RoundMax - 1);
}

FFreeKickTargetTable UMiniGameSubsystem::GetFreeKickTargetTable()
{
	int Round = FMath::Clamp(RoundIndex + 1, 1, RoundMax);
	int Stage = FMath::Clamp(StageIndex + 1, 1, StageMax);
	const FString RowString = FString::Printf(TEXT("%d-%d"), Round, Stage);
	/* 해당 줄에서 크래쉬 발생 자주 함. + (BallTargetActor.cpp:47) */
	const FFreeKickRoundTable RoundData = *DT_FreeKickRound->FindRow<FFreeKickRoundTable>(FName(*RowString), *RowString);
	
	const FString TargetRowString = FString::Printf(TEXT("%d"), FMath::RandRange(RoundData.Target_IDMin, RoundData.Target_IDMax));
	return *DT_FreeKickTarget->FindRow<FFreeKickTargetTable>(FName(*TargetRowString), *TargetRowString);
}
