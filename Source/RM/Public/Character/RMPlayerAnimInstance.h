// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundBase.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "DBTweenFloat.h"
#include "RMPlayerAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGestureGenderType : uint8
{
	ALL,
	MALE,
	FEMALE
};

USTRUCT(Blueprintable, BlueprintType)
struct FGestureSFX : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GestureSFX")
		int GestureID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GestureSFX")
		USoundBase* Sound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GestureSFX")
		EGestureGenderType SoundType;
};

USTRUCT(Blueprintable, BlueprintType)
struct FFootStepSoundSFX : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootStepSound")
		TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FootStepSound")
		USoundBase* Sound;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeFreeKickAction);

UCLASS()
class RM_API URMPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/* Native Func Override */
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaSeconds) override;
	virtual void NativeBeginPlay() override;
	virtual void BeginDestroy() override;

	/* C++ Only Function */
	class USoundConcurrency* GetGestureSoundConcurrency();
	class USoundAttenuation* GetGestureSoundAttenuation();


	/* UFUNCTION Function */

	/* Anim Notify Impl */
	/* 'AnimNotify_PlayGestureSound3D' is DEPRECATED 
	 * USE AnimNoify_RMPlaySound Directly(2022.04.09) 
	 */
	UFUNCTION()
		void AnimNotify_PlayGestureSound3D();
	UFUNCTION()
		void AnimNotify_PlayFootStepSound_L();
	UFUNCTION()
		void AnimNotify_PlayFootStepSound_R();
	
protected:
	/* C++ Only Function */
	/* Eye Blink Morph */
	UFUNCTION()
	virtual void UpdateEyeBlinkMorph(float weight);
	UFUNCTION()
	void StartEyeBlinkMorph();
	bool IsEyeBlinkTweenInitialized();
	void InitializeEyeBlinkTween();

	/* Init Sound Func */
	UFUNCTION(BlueprintCallable)
		void InitGestureSFX_Internal();
	UFUNCTION(BlueprintCallable)
		void InitFootStepSound_Internal();

	UFUNCTION()
		void PlayFootStepSound(bool bIsLeft);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Scale = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float HeadScale = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float HandScale = 1.0f;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Speed = 0.0f;
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector FacialBoneScale = FVector::OneVector;


	/* For Gesutre Sound */
	UPROPERTY(BlueprintReadOnly)
		class UBaseRMGameInstance* RMGameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USoundConcurrency* GestureSoundConcurrency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USoundAttenuation* GestureSoundAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int GestureSFXID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EGestureGenderType GenderType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsInitSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UDataTable* GestureSFXDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<int, USoundBase*> GestureSoundMap;

	/* For FootStep Sound */
	UPROPERTY()
		bool bIsInitFootStepSound = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		TEnumAsByte<EPhysicalSurface> CurrentSurface;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UDataTable* FootStepSoundTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> FootStepSoundMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool FreeKickMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TakeFreeKick;
	
	/* EyeBlink Anim Variable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EyeBlinkTimerRandMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EyeBlinkTimerRandMaxTime;
	/* Max Duration For one Blink Time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EyeBlinkTotalDuration;

private:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnTakeFreeKickAction OnTakeFreeKick;

	/* EyeBlink Anim Variable */
	/* C++ Base Variable */
	UPROPERTY()
	UDBTweenFloat* eyeBlinkTween;
	
	UPROPERTY()
	FTimerHandle eyeBlinkHandle;

	int tweenUpdateCount = 0;
	int tweenNameNumber = 0;
};
