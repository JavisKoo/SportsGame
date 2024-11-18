// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DartPin.generated.h"

UENUM(BlueprintType)
enum class EDARTPIN_STATE : uint8
{
	Unknown,
	Ready, Shoot, ShootEnd, ReadyToDestroy
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShootingFinished);

UCLASS()
class RM_API ADartPin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADartPin();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Public UFUNCTION Lib 
	UFUNCTION(BlueprintCallable)
		void Shoot(FVector EndLocation, float ShootingTimeDuration);
	UFUNCTION()
		void ResetPin();

	// getter
	EDARTPIN_STATE GetState() { return state; }
	UFUNCTION(BlueprintCallable)
		float GetDistFromCenter(FVector center);
	UFUNCTION(BlueprintCallable)
		void ShootingFinished();

	// setter
	void SetState(EDARTPIN_STATE s) { if (s != state) s = state; }
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = true))
		USceneComponent* rootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = true))
		USceneComponent* HitPoint;
	UPROPERTY(BlueprintAssignable)
		FOnShootingFinished OnShootFinished;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SplineComp", meta = (AllowPrivateAccess = true))
		class USplineComponent* splineComp;

protected:
	virtual void PostInitializeComponents() override;

	/* Play Dart Action Sound (0 = Shooting, 1 = Finished) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void PlayDartSound(const int soundIndex = 0);
	virtual void PlayDartSound_Implementation(const int soundIndex = 0);

protected:
	float distFromCenter;
	EDARTPIN_STATE state = EDARTPIN_STATE::Unknown;
private:
	UFUNCTION()
		void ShootingForNextTick();


private:
	UPROPERTY()
		FTimerHandle ShootingTimerHnd;
	UPROPERTY(EditAnywhere, Category = "Sound")
		class USoundWave* shootSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		class USoundWave* hitSound;

	bool bIsFloating = false;
	float currentShootingTime = 0.0f;
	float targetwholeTime = 0.0f;
	float restoreRotTime = 0.0f;

	float inputUpVectorForce = 0.0f;
	float inputRightVectorForce = 0.0f;
	const float maxUpVectorForce = 100.0f;
	const float maxRightVectorForce = 100.0f;
	const float maxUpVectorOffset = 20.0f;

	float rotatingRollValue = 0.0f;
	float offsetZ = 0.0f;
	const float shootingTick = 0.02f;

	// For Shoot Function Location Values
	FVector StartingLocation;
	FVector shootingTargetLocation;
	FVector breakPointLocation;

	// For Shoot Function Rotator Values
	FRotator StartingRotation;
	FRotator endRotation;
	FRotator breakPointRotation;

	// For Shoot Function Sclaed Values
	FVector StartingScale;
	FVector endScale;
	FVector breakPointScale;

	FVector toHitPointDirection;



};
