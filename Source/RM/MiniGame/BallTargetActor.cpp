// Fill out your copyright notice in the Description page of Project Settings.


#include "BallTargetActor.h"

const FString TweenName_MoveToTarget = FString("MoveToTarget");

// Sets default values
ABallTargetActor::ABallTargetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ABallTargetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABallTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallTargetActor::SetupTargetData()
{
	if (IsValid(SM_Target) == false)
	{
		TArray<UActorComponent*> StaticMeshes; 
		GetComponents(UStaticMeshComponent::StaticClass(), StaticMeshes);
		for (auto SM : StaticMeshes)
		{
			if (SM->GetName().Equals(FString("SM_Target")))
			{
				SM_Target = Cast<UStaticMeshComponent>(SM);
				break;
			}
		}
	}

	// 해당하는 라운드-스테이지 목표물 데이터 가져오기
	TargetData = GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->GetFreeKickTargetTable();

	// 목표물 크기 설정
	float scale = 1.0f;
	if (TargetData.TargetSize == EFreeKickTargetSize::Big)
		scale *= 1.0f;
	else if (TargetData.TargetSize == EFreeKickTargetSize::Middle)
		scale *= 0.65f;
	else
		scale *= 0.4f;

	// 목표물 초기 위치 설정
	SM_Target->SetWorldScale3D(FVector (scale, 1.0f, scale));
	this->SetActorLocationAndRotation(
		FVector(6940.0f, FMath::RandRange(TargetData.XLocationMin, TargetData.XLocationMax),  FMath::RandRange(TargetData.YLocationMin, TargetData.YLocationMax)),
		FRotator(0.0f, -90.0f, 0.0f),
		false, nullptr, ETeleportType::TeleportPhysics);

	// 움직임 초기화
	UDBTweenActor::DBTweenStop(*TweenName_MoveToTarget);

	// 기획단계에서 X(수평), Y(수직) 이동이라 헷갈리지 않도록 한번 대입해주고 계산한다
	float targetX = GetActorLocation().Y;
	float targetY = GetActorLocation().Z;
	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::Printf(TEXT("TargetX : %f"), targetX));
	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Black, FString::Printf(TEXT("TargetY : %f"), targetY));
	switch (TargetData.MoveType)
	{
		case EFreeKickTargetMoveType::None: break;
		case EFreeKickTargetMoveType::Horizontal:
			targetX = targetX < 0.0f ? targetX + TargetData.MoveDistance : targetX - TargetData.MoveDistance;
			break;
		case EFreeKickTargetMoveType::Vertical:
			targetY = targetY < 120.0f ? targetY + TargetData.MoveDistance : targetY - TargetData.MoveDistance;   
			break;
		case EFreeKickTargetMoveType::Diagonal:
			targetX = targetX < 0.0f	? targetX + TargetData.MoveDistance : targetX - TargetData.MoveDistance;
			targetY = targetY < 120.0f	? targetY + TargetData.MoveDistance : targetY - TargetData.MoveDistance;
			break;
	}

	if (TargetData.MoveType != EFreeKickTargetMoveType::None)
	{
		// X값 보정(화면 골대 밖으로 안나가게 설정)
		if (targetX < TargetData.XLocationMin)
			targetX = TargetData.XLocationMin;
		else if (targetX > TargetData.XLocationMax)
			targetX = TargetData.XLocationMax;

		// Y값 보정(화면 골대 밖으로 안나가게 설정)
		if (targetY < TargetData.YLocationMin)
			targetY = TargetData.YLocationMin;
		else if (targetY > TargetData.YLocationMax)
			targetY = TargetData.YLocationMax;

		// 움직임 시작
		UDBTweenActor::DOMove(*TweenName_MoveToTarget, TargetData.MoveDistance / TargetData.Speed, this, FVector(6940.0f, targetX, targetY), EaseType::Linear, true);
	}
		
}

