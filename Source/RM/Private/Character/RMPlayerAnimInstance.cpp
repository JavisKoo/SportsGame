#include "Character/RMPlayerAnimInstance.h"
#include "Engine/DataTable.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "../../BaseRMGameInstance.h"
#include "../../Public/Character/RMCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "DBTweenFloat.h"

/*
void URMPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	//this->GetCurveValue()
}
*/

void URMPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (!IsValid(FootStepSoundTable)) return;
	InitFootStepSound_Internal();
}

void URMPlayerAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);
	auto owner = Cast<ARMCharacter>(TryGetPawnOwner());
	bool bIsPlayerControlled = IsValid(owner) ? owner->IsPlayerControlled() : false;
	if (bIsPlayerControlled && owner)
		this->CurrentSurface = owner->CurrentSurface;

	if (!GetWorld()->GetTimerManager().IsTimerActive(eyeBlinkHandle))
	{
		float randTime = FMath::FRandRange(EyeBlinkTimerRandMinTime, EyeBlinkTimerRandMaxTime);
		GetWorld()->GetTimerManager().SetTimer(eyeBlinkHandle, FTimerDelegate::CreateUObject(this, &URMPlayerAnimInstance::StartEyeBlinkMorph), randTime, false);
	}
}

void URMPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	FString name = TEXT("OwingComp Name : ") + GetOwningComponent()->GetName();
	
	/* DEBUG */
	//if (UEngine* engine = GetWorld()->GetGameInstance()->GetEngine())
	//	engine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, *name);
	

	EyeBlinkTimerRandMinTime = FMath::Clamp<float>(EyeBlinkTimerRandMinTime, 2.0f, 4.0f);
	EyeBlinkTimerRandMaxTime = FMath::Clamp<float>(EyeBlinkTimerRandMaxTime, 6.0f, 10.0f);

	// Blink 타이머를 맞춰준다.
	float randTime = FMath::FRandRange(EyeBlinkTimerRandMinTime, EyeBlinkTimerRandMaxTime);
	GetWorld()->GetTimerManager().SetTimer(eyeBlinkHandle, FTimerDelegate::CreateUObject(this, &URMPlayerAnimInstance::StartEyeBlinkMorph), randTime, false);
}

void URMPlayerAnimInstance::BeginDestroy()
{
	if (eyeBlinkTween != nullptr)
	{
		if (eyeBlinkTween->OnUpdate.IsBound())
			eyeBlinkTween->OnUpdate.Clear();

		eyeBlinkTween->StopTweenSelf();
	}
		
	Super::BeginDestroy();
}

USoundConcurrency* URMPlayerAnimInstance::GetGestureSoundConcurrency()
{
	return GestureSoundConcurrency != nullptr ? GestureSoundConcurrency : nullptr;
}

USoundAttenuation* URMPlayerAnimInstance::GetGestureSoundAttenuation()
{
	return GestureSoundAttenuation != nullptr ? GestureSoundAttenuation : nullptr;
}

void URMPlayerAnimInstance::UpdateEyeBlinkMorph(float weight)
{
	const static float MIN = 0.0f;
	const static float MAX = 1.0f;
	weight = FMath::Clamp<float>(weight, MIN, MAX);
	if (tweenUpdateCount != 0 && weight == MIN || weight == MAX)
		tweenUpdateCount++;
	
	SetMorphTarget("EyeBlink", weight);

	if (tweenUpdateCount >= 2)
	{
		eyeBlinkTween->Paused = true;
		tweenUpdateCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(eyeBlinkHandle);
	}
}

void URMPlayerAnimInstance::StartEyeBlinkMorph()
{
	/* DEBUG */
	//if (UEngine* engine = GetWorld()->GetGameInstance()->GetEngine())
	//	engine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("URMPlayerAnimInstance::StartEyeBlinkMorph()"));

	if (!IsEyeBlinkTweenInitialized())
		InitializeEyeBlinkTween();
	else if(eyeBlinkTween->Paused)
	{
		/* DEBUG */
		//if (UEngine* engine = GetWorld()->GetGameInstance()->GetEngine())
		//	engine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Reset - URMPlayerAnimInstance::StartEyeBlinkMorph()"));
		eyeBlinkTween->ResetTweenSelf();
		eyeBlinkTween->PlayTweenSelf();
	}
}

bool URMPlayerAnimInstance::IsEyeBlinkTweenInitialized()
{
	/* DEBUG */
	//if (UEngine* engine = GetWorld()->GetGameInstance()->GetEngine())
	//	engine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("URMPlayerAnimInstance::IsEyeBlinkTweenInitialized()"));

	return eyeBlinkTween != nullptr;
}

void URMPlayerAnimInstance::InitializeEyeBlinkTween()
{
	/* DEBUG */
	//if (UEngine* engine = GetWorld()->GetGameInstance()->GetEngine())
	//	engine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("URMPlayerAnimInstance::InitializeEyeBlinkTween()"));
	
	const static float MAXAlpha = 1.0f;
	const static float MINAlpha = 0.0f;
	const float duration = FMath::Clamp<float>(EyeBlinkTotalDuration * 0.5f, 0.1f, 1.0f);

	//FString tweenName = TEXT("EyeBlink_Tween") + UKismetSystemLibrary::GetObjectName(TryGetPawnOwner()) + FString::FromInt(tweenNameNumber++);
	//eyeBlinkTween = UDBTweenFloat::DOFloat(tweenName, duration, MINAlpha, MAXAlpha, EaseType::Linear, true);
	//eyeBlinkTween->OnUpdate.AddDynamic(this, &URMPlayerAnimInstance::UpdateEyeBlinkMorph);
}

/* DEPRECATED --> USE AnimNoify_RMPlaySound Directly(2022.04.09) */
void URMPlayerAnimInstance::AnimNotify_PlayGestureSound3D()
{
	if (!bIsInitSFX)
		InitGestureSFX_Internal();

	if (USoundBase** sound = GestureSoundMap.Find(GestureSFXID))
	{
		auto owner = TryGetPawnOwner();
		if (owner != nullptr)
			RMGameInstance->PlaySFXSound3D(*sound, owner->GetActorLocation(), owner->GetActorRotation(), 
				1.0f, 1.0f, 0.0f, 
				GestureSoundAttenuation, GestureSoundConcurrency, owner);
	}
}

void URMPlayerAnimInstance::AnimNotify_PlayFootStepSound_L()
{
	auto owner = Cast<ARMCharacter>(TryGetPawnOwner());
	bool bIsPlayerControlled = IsValid(owner) ? owner->IsPlayerControlled() : false;

	/* ONLY PLAYER */
	if (bIsPlayerControlled)
		PlayFootStepSound(true);
}

void URMPlayerAnimInstance::AnimNotify_PlayFootStepSound_R()
{
	auto owner = Cast<ARMCharacter>(TryGetPawnOwner());
	bool bIsPlayerControlled = IsValid(owner) ? owner->IsPlayerControlled() : false;

	/* ONLY PLAYER */
	if (bIsPlayerControlled)
		PlayFootStepSound(false);
}

void URMPlayerAnimInstance::InitGestureSFX_Internal()
{
	if (GestureSFXDataTable == nullptr || bIsInitSFX) return;

	RMGameInstance = Cast<UBaseRMGameInstance>(GetWorld()->GetGameInstance());

	TArray<FName> outRow;
	UDataTableFunctionLibrary::GetDataTableRowNames(GestureSFXDataTable, outRow);
	for (FName row : outRow)
	{
		FString context;
		auto data = GestureSFXDataTable->FindRow<FGestureSFX>(row, context);
		if (data != nullptr)
		{
			if (data->SoundType == GenderType || data->SoundType == EGestureGenderType::ALL)
			{
				GestureSoundMap.Add(data->GestureID, data->Sound);
			}
		}
	}
	bIsInitSFX = true;
}

void URMPlayerAnimInstance::InitFootStepSound_Internal()
{
	if (!IsValid(FootStepSoundTable)) return;

	TArray<FName> outRow;
	UDataTableFunctionLibrary::GetDataTableRowNames(FootStepSoundTable, outRow);
	for (FName row : outRow)
	{
		FString context;
		if (auto data = FootStepSoundTable->FindRow<FFootStepSoundSFX>(row, context))
		{
			if (data->Sound != nullptr)
			{
				FootStepSoundMap.Add(data->SurfaceType, data->Sound);
			}
		}
	}
}

void URMPlayerAnimInstance::PlayFootStepSound(bool bIsLeft)
{
	if (FootStepSoundMap.Num() == 0)
	{
		InitFootStepSound_Internal();
	}
	
	if (USoundBase** sound = FootStepSoundMap.Find(CurrentSurface))
	{
		if (auto owner = Cast<ACharacter>(TryGetPawnOwner()))
		{
			RMGameInstance = RMGameInstance != nullptr ? RMGameInstance : Cast<UBaseRMGameInstance>(GetWorld()->GetGameInstance());
			FVector soundAtLocation = bIsLeft ? owner->GetMesh()->GetSocketLocation(TEXT("foot_l")) : owner->GetMesh()->GetSocketLocation(TEXT("foot_r"));
			RMGameInstance->PlaySFXSound3D(*sound, soundAtLocation, owner->GetActorRotation());
		}
	}
}

