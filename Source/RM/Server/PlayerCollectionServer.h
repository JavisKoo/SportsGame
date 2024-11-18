#pragma once

#include "PlayerCollectionServer.generated.h"

UENUM(BlueprintType)
enum class EPlayerCollectionMode : uint8
{
	None,
	PlayerAppearance,
	PlayerCollection
};

USTRUCT(BlueprintType)
struct FPlayerCollectionDateTime
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCollectionDateTime)
	int32 Hour = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCollectionDateTime)
	int32 Minute = 0;
};

UCLASS()
class RM_API APlayerCollectionServer : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerCollectionEndedEvent);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCollectionBeganEvent, const EPlayerCollectionMode, PlayerCollectionMode);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerCollectionChangedEvent, const EPlayerCollectionMode, PlayerCollectionMode, const int32, PlayerId, const float, RemainTime);

public:
	UFUNCTION(BlueprintCallable, Category = PlayerCollectionServer)
	void StartServer();

protected:
	void BeginPlay() override;

private:
	float GetTimePastFromBeginning();
	bool IsEndDailyPlayerCollection();
	void ClearCurrentTimer();
	void EnableTimerWhenStartServer(EPlayerCollectionMode& OutPlayerCollectionMode, float& OutRemainTime);
	void EndPlayerAppearTimer();
	void EndPlayerCollectionTimer();
	
	void BroadcastPlayerCollectionBegan(const EPlayerCollectionMode PlayerCollectionMode);
	void BroadcastPlayerCollectionEnded();
	void BroadcastPlayerCollectionChanged(const EPlayerCollectionMode PlayerCollectionMode, const int32 PlayerId, const float RemainTime);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCollectionServer)
	FPlayerCollectionDateTime ServerDailyStartTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCollectionServer)
	float PlayerAppearTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCollectionServer)
	float PlayerCollectionTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCollectionServer)
	int32 DailyPlayerCollectionCount = 0;

	UPROPERTY(BlueprintAssignable, Category = PlayerCollectionServer)
	FPlayerCollectionBeganEvent OnPlayerCollectionBegan;

	UPROPERTY(BlueprintAssignable, Category = PlayerCollectionServer)
	FPlayerCollectionEndedEvent OnPlayerCollectionEnded;

	UPROPERTY(BlueprintAssignable, Category = PlayerCollectionServer)
	FPlayerCollectionChangedEvent OnPlayerCollectionChanged;

private:
	FTimerManager* TimerManager = nullptr;
	FTimerHandle CurrentTimer;

	FDateTime ServerTodayStartDateTime;
	float PlayerCycleTime = 0.0f;
};
