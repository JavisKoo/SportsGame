#include "Task/Task.h"
ATask::ATask()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("Task");
}
void ATask::BeginPlay()
{
	Super::BeginPlay();
}
void ATask::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
