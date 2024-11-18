#pragma once

#include "CoreMinimal.h"
#include "RMDefinedEnum.h"
#include "RMNFTSubsystem.h"
#include "ThirdParty/Tween.h"
#include "GameFramework/Actor.h"
#include "NFTActor.generated.h"



UCLASS()
class RM_API ANFTActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ANFTActor();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBiddingState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Text;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fScale=0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fTurn=90.0f;


protected:
	UFUNCTION(BlueprintCallable)
	void Spawn(float To);

	UFUNCTION(BlueprintCallable)
	void Turn(float To);

	UFUNCTION(BlueprintCallable)
	void ChangeFSMState(ENFTActorState InNextState);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "NullState")
	void BP_NullState(EGameTickMode Mode, float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "DownloadingState")
	void BP_DownloadingState(EGameTickMode Mode, float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "LoopingState")
	void BP_LoopingState(EGameTickMode Mode, float DeltaTime);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	typedef void (ANFTActor::* TickFun)(EGameTickMode Mode, float DeltaTime);
	ENFTActorState CurrentState = ENFTActorState::ENull;
	TArray<TickFun> States;

	IwTween::TweenRepository Tween;
	TArray<ENFTActorState> NextStates;
};
