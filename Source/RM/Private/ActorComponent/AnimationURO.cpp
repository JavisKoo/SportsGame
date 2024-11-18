#include "ActorComponent/AnimationURO.h"

UAnimationURO::UAnimationURO()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAnimationURO::BeginPlay()
{
	Super::BeginPlay();
	if (FrameRate.Num() <= 0) return;
	//UActorComponent* pCpt = GetOwner()->GetComponentsByTag(USkinnedMeshComponent::StaticClass(), TEXT("Body"));
	TArray<UActorComponent*> A = GetOwner()->GetComponentsByTag(USkinnedMeshComponent::StaticClass(), TEXT("Body"));
	
	if (A.Num()>0)
	{
		USkinnedMeshComponent* pSKMesh = Cast<USkinnedMeshComponent>(A[0]);
		pSKMesh->bEnableUpdateRateOptimizations = true;
		pSKMesh->AnimUpdateRateParams->bShouldUseLodMap = true;
		for (int i = 0; i < FrameRate.Num(); i++)
		{
			pSKMesh->AnimUpdateRateParams->LODToFrameSkipMap.Add(i, FrameRate[i]);
		}
	}
}


