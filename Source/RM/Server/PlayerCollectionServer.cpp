#include "PlayerCollectionServer.h"
#include "RM/Library/PlayerCollectionFunctionLibrary.h"

APlayerCollectionServer::APlayerCollectionServer(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void APlayerCollectionServer::BeginPlay()
{
	Super::BeginPlay();

	TimerManager = &GetWorldTimerManager();
	PlayerCycleTime = PlayerAppearTime + PlayerCollectionTime;

	FDateTime NowDateTime = FDateTime::Now();
	ServerTodayStartDateTime = FDateTime(NowDateTime.GetYear(), NowDateTime.GetMonth(), NowDateTime.GetDay(), ServerDailyStartTime.Hour, ServerDailyStartTime.Minute, 0, 0);
}

void APlayerCollectionServer::StartServer()
{
	EPlayerCollectionMode PlayerCollectionMode = EPlayerCollectionMode::None;
	float TimerRemainTime = 0.0f;
	EnableTimerWhenStartServer(PlayerCollectionMode, TimerRemainTime);
	BroadcastPlayerCollectionBegan(PlayerCollectionMode);
}

void APlayerCollectionServer::ClearCurrentTimer()
{
	if (CurrentTimer.IsValid())
	{
		TimerManager->ClearTimer(CurrentTimer);
		CurrentTimer.Invalidate();
	}
}

void APlayerCollectionServer::EnableTimerWhenStartServer(EPlayerCollectionMode& OutPlayerCollectionMode, float& OutRemainTime)
{
	OutPlayerCollectionMode = EPlayerCollectionMode::None;
	OutRemainTime = 0.0f;

	ClearCurrentTimer();

	const float PastFromBeginningTime = GetTimePastFromBeginning();
	if (PastFromBeginningTime < 0.0f)
	{
		TimerManager->SetTimer(CurrentTimer, this, &APlayerCollectionServer::StartServer, PastFromBeginningTime * -1.0f);
	}
	else
	{
		float Quotient = FMath::CeilToFloat(PastFromBeginningTime) / PlayerCycleTime;
		if (Quotient >= (float)DailyPlayerCollectionCount)
		{
			BroadcastPlayerCollectionEnded();
		}
		else
		{
			float Remainder = FMath::Fmod(PastFromBeginningTime, PlayerCycleTime);
			if (Remainder < PlayerAppearTime)
			{
				float RemainTime = PlayerAppearTime - Remainder;
				// TODO. PlayerId를 어떻게 가져올지는 정해야 함. 일단 0으로 넘김 - ingsprinter
				int32 PlayerId = UPlayerCollectionFunctionLibrary::GetAcquirablePlayerId();
				BroadcastPlayerCollectionChanged(EPlayerCollectionMode::PlayerAppearance, 0, RemainTime);
				TimerManager->SetTimer(CurrentTimer, this, &APlayerCollectionServer::EndPlayerAppearTimer, RemainTime);

				OutPlayerCollectionMode = EPlayerCollectionMode::PlayerAppearance;
				OutRemainTime = RemainTime;
			}
			else
			{
				float RemainTime = PlayerCollectionTime - (Remainder - PlayerAppearTime);
				// TODO. PlayerId를 어떻게 가져올지는 정해야 함. 일단 0으로 넘김 - ingsprinter
				int32 PlayerId = UPlayerCollectionFunctionLibrary::GetAcquirablePlayerId();
				BroadcastPlayerCollectionChanged(EPlayerCollectionMode::PlayerCollection, 0, RemainTime);
				TimerManager->SetTimer(CurrentTimer, this, &APlayerCollectionServer::EndPlayerCollectionTimer, RemainTime);

				OutPlayerCollectionMode = EPlayerCollectionMode::PlayerCollection;
				OutRemainTime = RemainTime;
			}
		}
	}
}

void APlayerCollectionServer::EndPlayerAppearTimer()
{
	ClearCurrentTimer();

	if (IsEndDailyPlayerCollection())
	{
		BroadcastPlayerCollectionEnded();
	}
	else
	{
		// TODO. PlayerId를 어떻게 가져올지는 정해야 함. 일단 0으로 넘김 - ingsprinter
		int32 PlayerId = UPlayerCollectionFunctionLibrary::GetAcquirablePlayerId();
		BroadcastPlayerCollectionChanged(EPlayerCollectionMode::PlayerCollection, 0, PlayerCollectionTime);
		TimerManager->SetTimer(CurrentTimer, this, &APlayerCollectionServer::EndPlayerCollectionTimer, PlayerCollectionTime);
	}
}

void APlayerCollectionServer::EndPlayerCollectionTimer()
{
	ClearCurrentTimer();

	if (IsEndDailyPlayerCollection())
	{
		BroadcastPlayerCollectionEnded();
	}
	else
	{
		// TODO. PlayerId를 어떻게 가져올지는 정해야 함. 일단 0으로 넘김 - ingsprinter
		int32 PlayerId = UPlayerCollectionFunctionLibrary::GetAcquirablePlayerId();
		BroadcastPlayerCollectionChanged(EPlayerCollectionMode::PlayerAppearance, 0, PlayerAppearTime);
		TimerManager->SetTimer(CurrentTimer, this, &APlayerCollectionServer::EndPlayerAppearTimer, PlayerAppearTime);
	}
}

void APlayerCollectionServer::BroadcastPlayerCollectionBegan(const EPlayerCollectionMode PlayerCollectionMode)
{
	if (OnPlayerCollectionBegan.IsBound())
	{
		OnPlayerCollectionBegan.Broadcast(PlayerCollectionMode);
	}
}

void APlayerCollectionServer::BroadcastPlayerCollectionEnded()
{
	if (OnPlayerCollectionEnded.IsBound())
	{
		OnPlayerCollectionEnded.Broadcast();
	}	
}

void APlayerCollectionServer::BroadcastPlayerCollectionChanged(const EPlayerCollectionMode PlayerCollectionMode, const int32 PlayerId, const float RemainTime)
{
	if (OnPlayerCollectionChanged.IsBound())
	{
		OnPlayerCollectionChanged.Broadcast(PlayerCollectionMode, PlayerId, RemainTime);
	}
}

float APlayerCollectionServer::GetTimePastFromBeginning()
{
	FTimespan DiffDateTimespan = FDateTime::Now() - ServerTodayStartDateTime;
	return (float)(DiffDateTimespan.GetHours() * 3600) + (float)(DiffDateTimespan.GetMinutes() * 60) + (float)DiffDateTimespan.GetSeconds() + ((float)DiffDateTimespan.GetFractionMilli() / 1000.0f);
}

bool APlayerCollectionServer::IsEndDailyPlayerCollection()
{
	float Quotient = FMath::CeilToFloat(GetTimePastFromBeginning()) / PlayerCycleTime;
	return (Quotient >= (float)DailyPlayerCollectionCount);
}
