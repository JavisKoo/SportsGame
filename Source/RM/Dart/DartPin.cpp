// Fill out your copyright notice in the Description page of Project Settings.

#include "DartPin.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Sound/SoundWave.h"
//#include "iTween/iTween.h"
//#include "iTween/iTweenEvent.h"
/*
	TO Do: Shooting Action �ϼ���Ű��
*/

// Sets default values
ADartPin::ADartPin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	HitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HitPoint"));
	splineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	//movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));

	SetRootComponent(rootComp);
	mesh->SetupAttachment(rootComp);
	HitPoint->SetupAttachment(rootComp);
	splineComp->SetupAttachment(rootComp);
	

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	SM_DART_PIN(TEXT("StaticMesh'/Game/BackGround/First_Map/Common_Resource/MIniGame/Dart/DartMachine/SM_DartPin01_01.SM_DartPin01_01'"));
	if (SM_DART_PIN.Succeeded())
	{
		mesh->SetStaticMesh(SM_DART_PIN.Object);
		mesh->SetCastShadow(false);
		mesh->SetRelativeLocation(FVector(-3.5f, 0.0f, 0.0f));
	}
	SetActorEnableCollision(false);
	HitPoint->SetRelativeLocation(FVector(5.5f, 0.0f, 0.0f));
	
}

// Called when the game starts or when spawned
void ADartPin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADartPin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADartPin::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADartPin::PlayDartSound_Implementation(const int soundIndex)
{
	switch (soundIndex)
	{
		case 0:
			if (IsValid(shootSound))
				UGameplayStatics::PlaySound2D(GetWorld(), shootSound);
			break;
		case 1:
			if (IsValid(hitSound))
				UGameplayStatics::PlaySound2D(GetWorld(), hitSound);
			break;
		default:
			if (IsValid(shootSound))
				UGameplayStatics::PlaySound2D(GetWorld(), shootSound);
			break;
	}
}

float ADartPin::GetDistFromCenter(FVector center)
{
	FVector impactPoint = HitPoint->GetComponentLocation();
	return FVector::Dist(impactPoint, center);
}

void ADartPin::ShootingFinished()
{
	if (OnShootFinished.IsBound())
		OnShootFinished.Broadcast();
	PlayDartSound(1);
}



/// <summary>
/// ��Ʈ���� �߻��մϴ�.
/// </summary>
/// <param name="EndLocation">���� ���� Depth������ ��ǥ ��ġ ��</param>
/// <param name="ShootingTimeDuration">�� ���ư� �ð�</param>
/// <param name="upForce">�������� �󸶸�ŭ �� �ö��� Force ����Ʈ�� 0</param>
/// <param name="rightForce">���������� �󸶸�ŭ �� �̵����� ������ �� ����Ʈ�� 0</param>
void ADartPin::Shoot(FVector EndLocation, float ShootingTimeDuration)
{
	// 1) Ÿ�̸Ӱ� �ߵ��Ǿ� ������ �Լ��� ��������.
	if (GetWorldTimerManager().IsTimerActive(ShootingTimerHnd))
		return;

	// forward Offset (O)
	FVector RealForward = GetActorForwardVector().RotateAngleAxis(GetActorRotation().Pitch, GetActorRightVector());
	EndLocation += RealForward * FVector::Dist(GetActorLocation(), HitPoint->GetComponentLocation()) * -1.0f / GetActorScale3D().X;

	StartingLocation = GetActorLocation();
	StartingRotation = GetActorRotation();
	StartingScale = GetActorScale3D();
	shootingTargetLocation = EndLocation;
	currentShootingTime = 0.0f;
	targetwholeTime = ShootingTimeDuration;
	restoreRotTime = ShootingTimeDuration * 0.5f;
	bIsFloating = true;


	// 2) ������ ZValue �� �� 
	float offsetZValue = EndLocation.Z - GetActorLocation().Z;
	FVector tmpDirection = EndLocation - GetActorLocation();
	tmpDirection.Normalize();
	breakPointLocation = GetActorLocation();

	if (offsetZValue >= 0)
		breakPointLocation += tmpDirection * (FVector::Dist(GetActorLocation(), EndLocation) * 0.5f) + (FVector::UpVector * 2.0f);
	else
		breakPointLocation += tmpDirection * (FVector::Dist(GetActorLocation(), EndLocation) * 0.5f);

	breakPointRotation = FRotator(0.0f, GetActorRotation().Yaw, 180.0f);
	endRotation = FRotator(0.0f, GetActorRotation().Yaw, 360.0f);
	breakPointScale = StartingScale / 2;
	endScale = FVector(1.0f);


	//AiTweenEvent* UiTween::ActorMoveToSplinePointFull(float timerInterval, FName tweenName /*= "No Name"*/, 
	//AActor* actorToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool moveToPath /*= false*/,
	//bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints /*= VectorConstraints::none*/, bool sweep /*= false*/, float delay /*= 0.0f*/, 
	//EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/,
	//UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections,
	//bool switchPathOrientationDirection, ELookType::LookType orientation /*= noOrientationChange*/, UObject* orientationTarget /*= nullptr*/, float orientationSpeed /*= 5.0f*/, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, 
	//UObject* onTweenStartTarget /*= nullptr*/, FString OnTweenStartFunctionName /*= ""*/, UObject* onTweenUpdateTarget /*= nullptr*/, FString OnTweenUpdateFunctionName /*= ""*/, UObject* onTweenLoopTarget /*= nullptr*/, FString OnTweenLoopFunctionName /*= ""*/, UObject* onTweenCompleteTarget /*= nullptr*/, FString OnTweenCompleteFunctionName /*= ""*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*) = false*/, bool destroySplineComponent, bool tieToObjectValidity, bool cullNonRenderedTweens, float secondsToWaitBeforeCull)


	//if (splineComp != nullptr)
	//{
	//	splineComp->ClearSplinePoints(true);
	//	splineComp->AddSplinePoint(breakPointLocation, ESplineCoordinateSpace::World);
	//	splineComp->AddSplinePoint(EndLocation, ESplineCoordinateSpace::World);
	//}

	//AiTweenEvent* ie = UiTween::ActorMoveToSplinePointFull(0.0f, "No Name",
	//	this, splineComp, true, true, EVectorConstraints::none,
	//	false, 0.0f, EDelayType::first, ETickType::seconds, ShootingTimeDuration,
	//	EEaseType::linear, nullptr, 1.0f, ELoopType::once, 0,
	//	true, ELookType::noOrientationChange, nullptr, 5.0f, ERotatorConstraints::pitchOnly,
	//	nullptr, "", nullptr, "", nullptr, "", this, "ShootingFinished");

	GetWorldTimerManager().SetTimer(ShootingTimerHnd, FTimerDelegate::CreateUObject(this, &ADartPin::ShootingForNextTick), shootingTick, true);
	PlayDartSound(0);
	/*UE_LOG(LogTemp, Warning, TEXT("BreakPointLocation X : %.2f, Y : %.2f, Z : %.2f"), breakPointLocation.X, breakPointLocation.Y, breakPointLocation.Z);
	UE_LOG(LogTemp, Warning, TEXT("EndLocation X : %.2f, Y : %.2f, Z : %.2f"), EndLocation.X, EndLocation.Y, EndLocation.Z);*/
}

void ADartPin::ShootingForNextTick()
{
	// 1) EndLocation ���� �������� �׸��鼭 ���ư���.
	// 1-1) sin �׷����� �׸��鼭 ���ư���?
	currentShootingTime += shootingTick;
	//UE_LOG(LogTemp, Warning, TEXT("CurrentShootingTime : %.2f"), currentShootingTime);
	
	//UGameplayStatics::BlueprintSuggestProjectileVelocity(GetWorld(), )

	if (currentShootingTime > targetwholeTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentShootingTime : %.2f     TargetWholeTime : %.2f"), currentShootingTime, targetwholeTime);
		SetActorRotation(FMath::Lerp(StartingRotation, endRotation, 1.0f));
		SetActorLocation(FMath::Lerp(StartingLocation, shootingTargetLocation, 1.0f));
		bIsFloating = false;
		GetWorldTimerManager().ClearTimer(ShootingTimerHnd);
		ShootingFinished();
		return;
	}

	static FRotator restoreRot;
	float lerpAlpha = 0.0f;
	if (currentShootingTime < restoreRotTime)
	{
		restoreRot = breakPointRotation;
		lerpAlpha = currentShootingTime / restoreRotTime;
		SetActorRotation(FMath::Lerp(StartingRotation, restoreRot, lerpAlpha));
		SetActorLocation(FMath::Lerp(StartingLocation, breakPointLocation, lerpAlpha));
		SetActorScale3D(FMath::Lerp(StartingScale, breakPointScale, lerpAlpha));
	}
	else
	{
		StartingRotation = breakPointRotation;
		StartingLocation = breakPointLocation;
		StartingScale = breakPointScale;
		restoreRot = endRotation;
		lerpAlpha = (currentShootingTime - restoreRotTime) / (targetwholeTime - restoreRotTime);
		SetActorRotation(FMath::Lerp(StartingRotation, restoreRot, lerpAlpha));
		SetActorLocation(FMath::Lerp(StartingLocation, shootingTargetLocation, lerpAlpha));	
		SetActorScale3D(FMath::Lerp(StartingScale, endScale, lerpAlpha));
		mesh->SetCastShadow(true);
	}
}

/// <summary>
/// Pin�� �ٽ� ������ ���� setting �Լ�
/// </summary>
void ADartPin::ResetPin()
{
	currentShootingTime = 0.0f;
	targetwholeTime = 0.0f;

	// Rotation
	SetActorRotation(FRotator(30.0f, GetActorRotation().Yaw, 0.0f));
	breakPointRotation = FRotator(0.0f, GetActorRotation().Yaw, 180.0f);
	endRotation = FRotator(0.0f, GetActorRotation().Yaw, 360.0f);

	// scale
	endScale = FVector(1.0f);

	// SetActor Hidden
	SetActorHiddenInGame(true);
	mesh->SetCastShadow(false);
}

