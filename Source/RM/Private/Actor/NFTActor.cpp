#include "Actor/NFTActor.h"

using namespace IwTween;

ANFTActor::ANFTActor()
{
	PrimaryActorTick.bCanEverTick = true;
	States.Add(&ANFTActor::BP_NullState);
	States.Add(&ANFTActor::BP_DownloadingState);
	States.Add(&ANFTActor::BP_LoopingState);
}
void ANFTActor::Spawn(float To) {
	Tween.Run(0.25, eEasing, Ease::backOut, eFloat, &fScale, To, eEnd);
}

void ANFTActor::Turn(float To) {
	Tween.Run(0.5, eEasing, Ease::backOut, eFloat, &fTurn, To, eEnd);
}


void ANFTActor::ChangeFSMState(ENFTActorState InNextState)
{
	NextStates.Add(InNextState);
}

void ANFTActor::BeginPlay()
{
	Super::BeginPlay();
}

void ANFTActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (NextStates.Num() > 0) {
		ENFTActorState NextState = NextStates[0];
		(this->*(States[(uint8)CurrentState]))(EGameTickMode::Leave, 0.0f);
		(this->*(States[(uint8)NextState]))(EGameTickMode::Enter, 0.0f);
		CurrentState = NextState;
		NextStates.RemoveAt(0);
	}
	(this->*(States[(uint8)CurrentState]))(EGameTickMode::Tick, DeltaTime);
	Tween.Tick(DeltaTime);
}

