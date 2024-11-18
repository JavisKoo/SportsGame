// Fill out your copyright notice in the Description page of Project Settings.

#include "DartBoard.h"
#include "Widget_DartGame.h"
#include "Camera/CameraActor.h"
#include "DrawDebugHelpers.h"
/*
	1) Ŭ���ϸ� DartPin State Ready�� ����
	2) Release ���� �� -> ���ǿ� ������ State Shoot���� ����
		- 2-1) �Լ� ȣ���̳� ��������Ʈ��?
		- 2-2) 

*/

const int MAX_DARTPIN_INDEX = 2;

ADartBoard::ADartBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create Component
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardCollision"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("DartCamera"));
	roundEndCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("RoundEndCamera"));
	DartpinSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("PinSpawnLocation"));
	DartBoardInfo = CreateDefaultSubobject<USphereComponent>(TEXT("BoardInfo"));


	// Setup Attachement & Relative Location
	SetRootComponent(mesh);
	boxCollision->SetupAttachment(mesh);
	boxCollision->SetRelativeLocation(FVector(0.0f, 25.f, 0.0f));
	boxCollision->SetBoxExtent(FVector(300.0f, 0.001f, 300.0f));
	boxCollision->SetCollisionProfileName(FName(TEXT("Trigger")));
	//boxCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);

	camera->SetupAttachment(mesh);
	camera->SetRelativeLocationAndRotation(FVector(0.0f, 205.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));
	camera->SetFieldOfView(30.0f);
	camera->bConstrainAspectRatio = false;

	roundEndCamera->SetupAttachment(camera);
	roundEndCamera->SetRelativeLocationAndRotation(FVector(105.f, -142.f, 0.0f), FRotator(0,55.0f,0));

	DartpinSpawnLocation->SetupAttachment(camera);
	DartpinSpawnLocation->SetRelativeLocation(FVector(30.0f, 0.0f, -25.0f));
	
	DartBoardInfo->SetupAttachment(RootComponent);
	DartBoardInfo->SetRelativeLocation(FVector(0.0f, 2.0f, 0.0f));
	DartBoardInfo->SetSphereRadius(27.5f);
	DartBoardInfo->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SM_DARTBOARD(TEXT("StaticMesh'/Game/BackGround/First_Map/Common_Resource/MIniGame/Dart/DartMachine/SM_DartBoard01_01.SM_DartBoard01_01'"));
	if (SM_DARTBOARD.Succeeded())
	{
		mesh->SetStaticMesh(SM_DARTBOARD.Object);
	}

	// SetSocketNames
	SetUpSocketNames(TEXT("CenterPos"));
	SetUpSocketNames(TEXT("OuterBullsEyeStartPos"));
	SetUpSocketNames(TEXT("OuterBullsEyeEndPos"));
	SetUpSocketNames(TEXT("TrebleStartPos"));
	SetUpSocketNames(TEXT("TrebleEndPos"));
	SetUpSocketNames(TEXT("DoubleStartPos"));
	SetUpSocketNames(TEXT("DoubleEndPos"));
	SetUpSocketNames(TEXT("RightEndPos"));
	SetUpSocketNames(TEXT("LeftEndPos"));
	
	SetUpDefaultDistance();

	// Set SubClasses
	DartPinActorClass = ADartPin::StaticClass();
}

// Called when the game starts or when spawned
void ADartBoard::BeginPlay()
{
	Super::BeginPlay();

	boxCollision->UpdateOverlaps();
}

void ADartBoard::EndPlay(const EEndPlayReason::Type reason)
{
	GLog->Log("Overriding th endplay Function In DartBoard Actor.");
	TArray<AActor*> AllPins;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADartPin::StaticClass(), AllPins);
	if (AllPins.Num() > 0)
	{
		GLog->Log("Find Not Catched DartPin Actor.");
		for (AActor* pin : AllPins)
			pin->Destroy();
	}
	Super::EndPlay(reason);
	
}

// Called every frame
void ADartBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADartBoard::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	centerPos = mesh->GetSocketLocation(TEXT("CenterPos"));
	SetUpDirections(TEXT("RIghtEndpos"), TEXT("LeftEndPos"));
	SetUpDefaultDistance();
}

UWidget_DartGame* ADartBoard::GetDartGameWidget()
{
	return (dartGameWidget == nullptr) ? nullptr : dartGameWidget;
}

void ADartBoard::SetUpSocketNames(FName socketname)
{
	if (!IsValid(mesh) || !mesh->DoesSocketExist(socketname) || SocketNames.Contains(socketname)) return;

	SocketNames.Add(socketname);
}


void ADartBoard::SetUpDirections(FName RightEndPosName, FName LeftEndPosName)
{
	if (!IsValid(mesh->GetStaticMesh()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find DartBoard->StaticMesh"));
		return;
	}

}

/// <summary>
/// ��������� ���� dist �¾�
/// Called When Actor BeginPlay Function
/// </summary>
void ADartBoard::SetUpDefaultDistance()
{
	if (!IsValid(mesh->GetStaticMesh()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find DartBoard->StaticMesh"));
		return;
	}
	distBullsEye = 1.2f;
	distOuterBullsEye = 2.25f;
	distTrebleStart = 13.2f;
	distTrebleEnd = 14.5f;
	distDoubleStart = 21.9f;
	distDoubleEnd = 23.5f;
}

void ADartBoard::StartGame()
{
	if (DartGameWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Must Set DartGameWidget Class!!"));
		return;
	}

	auto playerController = GetWorld()->GetFirstPlayerController();
	//playerController->SetViewTarget(this);

	// ����Ͽ����� �ٲ� ���� ��ġ�� �ٲ�� ��.
#if PLATFORM_WINDOWS
	playerController->bShowMouseCursor = true;
#endif
	dartGameWidget = CreateWidget<UWidget_DartGame>(GetWorld(), DartGameWidgetClass);
	dartGameWidget->AddToViewport(1);
	dartGameWidget->BindInfo(this);
	dartGameWidget->OnDartChangedPositionDelegate.AddDynamic(this, &ADartBoard::UpdateDartPinLocation);
	dartGameWidget->OnExitBtnClickedDelegate.AddDynamic(this, &ADartBoard::ExitFromDartGame);
	// Play dart Game 
	//GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorHiddenInGame(true);

}


void ADartBoard::SpawnDartPin(FVector SpawnLocation3D, FVector SpawnScale3D)
{
	if (DartPinActorClass == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("Can't Find DartPin Actor Class!!"));
		return;
	}
	
	if (spawnedDartpin != nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("Dartpin Actor Already Spawned!!"));
		return;
	}

	FTransform spawnTransform = DartpinSpawnLocation->GetComponentTransform();
	spawnTransform.SetLocation(SpawnLocation3D);
	spawnTransform.SetScale3D(SpawnScale3D);
	if (spawnedDartPinsArr.IsValidIndex(currentDartpinIndex))
	{
		spawnedDartpin = spawnedDartPinsArr[currentDartpinIndex];
		spawnedDartpin->SetActorTransform(spawnTransform);
		spawnedDartpin->SetActorHiddenInGame(false);
	}
	else
	{
		spawnedDartpin = GetWorld()->SpawnActor<ADartPin>(DartPinActorClass, spawnTransform);
		spawnedDartpin->OnShootFinished.AddDynamic(this, &ADartBoard::ShootFinished);
	}
	spawnedDartpin->SetActorRelativeRotation(FRotator(30, spawnedDartpin->GetActorRotation().Yaw, spawnedDartpin->GetActorRotation().Roll));
}

void ADartBoard::ShootDartPin(FVector StartPos, FVector EndPos)
{
	if (spawnedDartpin == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("Dartpin Actor not Spawned yet."));
		return;
	}
	//const float alphaForce = 1.25f;
	const float outofRange = DartBoardInfo->GetScaledSphereRadius();
	
	FVector direction = EndPos - StartPos;	// �밢�� ����
	FVector offsetDirection = DartBoardInfo->GetComponentLocation() - boxCollision->GetComponentLocation();		// �� ����
	float offsetDist = offsetDirection.Size();
	offsetDirection.Normalize();

	offsetDirection *= offsetDist;
	FVector ConvertedEndLocation = (EndPos + offsetDirection);
	
	// �� ������ ������ �� ó��
	float distFromCenter = FVector::Dist(DartBoardInfo->GetComponentLocation(), ConvertedEndLocation);
	if (distFromCenter > outofRange)
	{
		FHitResult result;
		FCollisionQueryParams collisionParam;
		TArray<TWeakObjectPtr<const AActor>> weakptrArr;
		weakptrArr.Add(this);
		for (ADartPin* pin : spawnedDartPinsArr)
		{
			TWeakObjectPtr<AActor> weakptr = Cast<AActor>(pin);
			if (weakptr.IsValid())
				weakptrArr.Add(weakptr);
		}

		collisionParam.AddIgnoredActors(weakptrArr);
		GetWorld()->LineTraceSingleByChannel(result, EndPos, ConvertedEndLocation + offsetDirection * 100, ECollisionChannel::ECC_Camera, collisionParam);
		if (result.bBlockingHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("GetHitLocation, Hit Actor : %s,  ImpactPoint = %s"), *result.GetActor()->GetName(), *result.Location.ToString());
			ConvertedEndLocation = result.Location;;
			
		}

	}

	spawnedDartpin->Shoot(ConvertedEndLocation, 0.3f);
	/*float distFromCenter = (ConvertedEndLocation - DartBoardInfo->GetComponentLocation()).Size();*/
	/*UE_LOG(LogTemp, Error, TEXT("X : %.2f,  Y : %.2f,  Z : %.2f"), ConvertedEndLocation.X, ConvertedEndLocation.Y, ConvertedEndLocation.Z);*/
}


void ADartBoard::ShootDartPinWithPressedTime(FVector StartPos, FVector EndPos, float PressedTimeSecond)
{
	// ���� �� ������ ���
	FVector2D Offset = GetRandomRangeOffset(PressedTimeSecond);
	FVector rightDirection = mesh->GetSocketLocation(TEXT("RightEndPos")) - mesh->GetSocketLocation(TEXT("CenterPos"));
	rightDirection.Normalize();

	// EndPos ���� (RandomRange)
	EndPos += rightDirection * Offset.X;
	EndPos += GetActorUpVector() * Offset.Y;

#if ENABLE_DRAW_DEBUG && UE_EDITOR
	// For Deubging IN EDITOR
	static FVector OffsetFront = DartBoardInfo->GetComponentLocation() - boxCollision->GetComponentLocation();
	static FVector debugPoint;
	const static float drawTime = 3.0f;

	debugPoint = EndPos + OffsetFront;
	DrawDebugPointOriginAndOffsetPoint(debugPoint, drawTime);
#endif

	// ������ �ִ� �ð��� ���� offset ��
	float offsetZ = FMath::Pow(PressedTimeSecond, 2) * multipleOffsetZValue / ((divideOffsetZValue >= 1.0f) ? divideOffsetZValue : 1.0f);
	EndPos += GetActorUpVector() * offsetZ;


	ShootDartPin(StartPos, EndPos);
}

void ADartBoard::ShootFinished()
{
	if (spawnedDartpin == nullptr)
		return;

	FVector impactPoint = spawnedDartpin->HitPoint->GetComponentLocation();
	int score = GetScore(impactPoint);
	UE_LOG(LogTemp, Error, TEXT("Score : %d"), score);

	if (!spawnedDartPinsArr.IsValidIndex(currentDartpinIndex))
		spawnedDartPinsArr.Add(spawnedDartpin);

	spawnedDartpin = nullptr;
	currentDartpinIndex++;
	if (currentDartpinIndex > MAX_DARTPIN_INDEX)
		currentDartpinIndex = 0;

	if (onScoredDelegate.IsBound())
		onScoredDelegate.Broadcast(score);
}

int ADartBoard::GetScore(FVector impactPoint)
{
	// �߾ӿ��������� �Ÿ�
	float distFromCenter = FVector::Dist(DartBoardInfo->GetComponentLocation(), impactPoint);
	
	// �߾ӿ������� ��Ʈ�� ��ġ������ ����
	FVector directionForPoint = impactPoint - DartBoardInfo->GetComponentLocation();
	directionForPoint.Normalize();
	
	// ī�޶� ���� ������ ����
	FVector rightDirection = mesh->GetSocketLocation(TEXT("RightEndPos")) - mesh->GetSocketLocation(TEXT("CenterPos"));
	rightDirection.Normalize();

	// ī�޶� ���� ������ ���͸� �������� ���� ���
	float angle = FVector::DotProduct(rightDirection, directionForPoint);
	angle = FMath::Acos(angle);
	angle = FMath::RadiansToDegrees(angle);
	
	// ���� �� = 0 ~ 180�̱� ���������� �������� ������ �˱� ���� ���
	FVector downDirection = DartBoardInfo->GetUpVector() * -1.0f;
	float dotValueFromDownDirection = FVector::DotProduct(downDirection, directionForPoint);
	
	// ������
	if (dotValueFromDownDirection > -KINDA_SMALL_NUMBER)
		angle *= -1.0f;

	// �� ���� Dist �Ÿ��� ��
	float scaledValue = GetActorScale3D().X;
	if (distFromCenter <= distBullsEye * scaledValue)
		return 50;
	else if (distFromCenter <= distOuterBullsEye * scaledValue)
		return 25;
	else if (distFromCenter <= distTrebleStart * scaledValue)
		return GetScoreFromAngle(angle);
	else if (distFromCenter <= distTrebleEnd * scaledValue)
		return GetScoreFromAngle(angle) * 3;
	else if (distFromCenter <= distDoubleStart * scaledValue)
		return GetScoreFromAngle(angle);
	else if (distFromCenter <= distDoubleEnd * scaledValue)
		return GetScoreFromAngle(angle) * 2;

	return 0;
}

int ADartBoard::GetScoreFromAngle(float angle)
{
	// 20���
	//360 / 20 = 18degree

	UE_LOG(LogTemp, Warning, TEXT("Angle: %.2f"), angle);
	const int stdAngleForHalf = 180 / 20;
	int angleCount = 0;
	if (angle < -KINDA_SMALL_NUMBER)
	{
		angle *= -1.0f;
		for (int i = 1; i <= 11; i++)
		{
			if (i == 1 || i == 11)
				angleCount++;
			else
				angleCount += 2;

			int cmpAngle = stdAngleForHalf * angleCount;
			if (angle <= cmpAngle)
			{
				switch (i)
				{
					case 1:
						return 6;
					case 2:
						return 10;
					case 3:
						return 15;
					case 4:
						return 2;
					case 5:
						return 17;
					case 6:
						return 3;
					case 7:
						return 19;
					case 8:
						return 7;
					case 9:
						return 16;
					case 10:
						return 8;
					case 11:
						return 11;
				}
			}
		}
	}
	else
	{
		for (int i = 1; i <= 11; i++)
		{
			if (i == 1 || i == 11)
				angleCount++;
			else
				angleCount += 2;

			int cmpAngle = stdAngleForHalf * angleCount;
			if (angle <= cmpAngle)
			{
				switch (i)
				{
					case 1:
						return 6;
					case 2:
						return 13;
					case 3:
						return 4;
					case 4:
						return 18;
					case 5:
						return 1;
					case 6:
						return 20;
					case 7:
						return 5;
					case 8:
						return 12;
					case 9:
						return 9;
					case 10:
						return 14;
					case 11:
						return 11;
				}
			}
		}
	}

	return 20;
}

void ADartBoard::SetBlendTargetRoundEndCam(float blendTime)
{
	auto playerController = GetWorld()->GetFirstPlayerController();
	if (playerController == nullptr) return;
	
	if (!roundEndSpawnedCam.IsValid())
	{
		roundEndSpawnedCam = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), roundEndCamera->GetComponentLocation(), roundEndCamera->GetComponentRotation());
		roundEndSpawnedCam->GetCameraComponent()->bConstrainAspectRatio = true;
		roundEndSpawnedCam->GetCameraComponent()->SetAspectRatio(camera->AspectRatio);
		roundEndSpawnedCam->GetCameraComponent()->SetFieldOfView(camera->FieldOfView);
	}

	if (!roundEndSpawnedCam.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("RoundEndSpawnedCam Is Not Valid!, Try Again."));
		return;
	}

	playerController->SetViewTargetWithBlend(roundEndSpawnedCam.Get(), blendTime, EViewTargetBlendFunction::VTBlend_Cubic);
	
	// SetTimer For End of BlendViewTarget.
	GetWorldTimerManager().SetTimer(viewTargetHandle, FTimerDelegate::CreateUObject(this, &ADartBoard::EndBlendViewTarget), 0.1f, false, blendTime);
}

void ADartBoard::SetBlendTargetMainCam(float blendTime)
{
	auto playerController = GetWorld()->GetFirstPlayerController();
	if (playerController == nullptr) return;

	playerController->SetViewTargetWithBlend(this, blendTime, EViewTargetBlendFunction::VTBlend_Cubic);
	GetWorldTimerManager().SetTimer(viewTargetHandle, FTimerDelegate::CreateUObject(this, &ADartBoard::EndBlendViewTarget), 0.1f, false, blendTime);
}

void ADartBoard::ResetRound()
{
	currentDartpinIndex = 0;
	for (ADartPin* actor : spawnedDartPinsArr)
	{
		actor->SetActorHiddenInGame(true);
		actor->ResetPin();
	}
}

void ADartBoard::ExitFromDartGame()
{
	ResetRound();
	
	for (ADartPin* pin : spawnedDartPinsArr)
		pin->Destroy();
	spawnedDartPinsArr.Empty();

	if (IsValid(spawnedDartpin))
	{
		spawnedDartpin->Destroy();
		spawnedDartpin = nullptr;
	}
	
	auto playerController = GetWorld()->GetFirstPlayerController();
//	if (playerController != nullptr)
//	{
//		//if (IsValid(playerController->GetPawn()))
//		//{
//		//	playerController->GetPawn()->SetActorHiddenInGame(false);
//		//}
//#if PLATFORM_WINDOWS
//		playerController->bShowMouseCursor = false;
//#endif
//	}

	if (dartGameWidget != nullptr)
	{
		onScoredDelegate.Clear();
		dartGameWidget->RemoveFromViewport();
		dartGameWidget = nullptr;
	}
}

void ADartBoard::EndBlendViewTarget()
{
	if (onEndOfBlendViewTargetDelegate.IsBound())
		onEndOfBlendViewTargetDelegate.Broadcast();

	onEndOfBlendViewTargetDelegate.Clear();
	GetWorldTimerManager().ClearTimer(viewTargetHandle);
}

void ADartBoard::DrawRandomRange(FVector currentMousePositionToWorldPos, FColor color, float drawTime, float PressedTimeDelta)
{
	static FVector rightDirection = mesh->GetSocketLocation(TEXT("RightEndPos")) - mesh->GetSocketLocation(TEXT("CenterPos"));
	rightDirection.Normalize();
	static FVector OffsetDepth = DartBoardInfo->GetComponentLocation() - boxCollision->GetComponentLocation();
	currentMousePositionToWorldPos += OffsetDepth;

	const float radius = maxRangeOfRandomRadius - multipleValueOfPressedTime * PressedTimeDelta;
	const static float lineTickness = 0.5f;
	DrawDebugCircle(GetWorld(), currentMousePositionToWorldPos, radius, 50, color, false, drawTime, 0, lineTickness, rightDirection);
}

void ADartBoard::DrawDebugPointOriginAndOffsetPoint(FVector OriginPoint, float drawTime)
{
	static const float size = 3.0f;
	DrawDebugPoint(GetWorld(), OriginPoint, size, FColor::Blue, false, drawTime);
}


void ADartBoard::UpdateDartPinLocation(FVector newLocation, bool bTeleportPosition)
{
	if (spawnedDartpin == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("Dartpin Actor not Spawned yet"));
		return;
	}

	static float lerpDeltaTime = 1.0f;
	static float lerpDeltaSpeed = 0.1f;
	FVector lerpLocation = FMath::VInterpTo(spawnedDartpin->GetActorLocation(), newLocation, lerpDeltaTime, lerpDeltaSpeed);

	spawnedDartpin->SetActorLocation(bTeleportPosition ? newLocation : lerpLocation);
}

FVector2D ADartBoard::GetRandomRangeOffset(float PressedTimeSec)
{
	// y = 12 - 4 * PressedTimeSecond
	static const float DefaultreturnVal = maxRangeOfRandomRadius;
	static const float MultipleX = multipleValueOfPressedTime;
	const float radius = DefaultreturnVal - MultipleX * PressedTimeSec;
	float rightVecOffset = FMath::RandRange(-1.0f * radius, radius);
	float upVecOffset = FMath::RandRange(-1.0f * radius, radius);

	return FVector2D(rightVecOffset, upVecOffset);
}

FName ADartBoard::GetSocketNameByIndex(int index)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDART_SOCKET_NAMES"), true);
	if (!enumPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't Find UENUM Classes!"));
		return FName(TEXT("INVALID"));
	}
	
	return enumPtr->GetNameByIndex(index);
}

EDART_SOCKET_NAMES ADartBoard::GetSocketEnumByName(FName name)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDART_SOCKET_NAMES"), true);
	if (!enumPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't Find UENUM Classes!"));
		return EDART_SOCKET_NAMES::NONE;
	}
	return (EDART_SOCKET_NAMES)enumPtr->GetValueByName(name);
}


// Test �ּ� �߰�