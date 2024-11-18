// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "DartPin.h"
#include "DartBoard.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVoidDelVoidDYN);
DECLARE_MULTICAST_DELEGATE(FVoidDelVoid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetScoreDelegate, int, GetScored);

UENUM(BlueprintType)
enum class EDART_SOCKET_NAMES : uint8
{
	CenterPos = 0,
	OuterBullsEyeStartPos, OuterBullsEyeEndPos, TrebleStartPos, TrebleEndPos, DoubleStartPos, DoubleEndPos,
	RightEndPos, LeftEndPos,
	NONE
};


UCLASS()
class RM_API ADartBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADartBoard();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type reason) override;
	virtual void PostInitializeComponents() override;
	

	// GetterFunction
	UFUNCTION(BlueprintCallable)
		UCameraComponent* GetCameraComponent() { return camera; }
	UFUNCTION(BlueprintCallable)
		AActor* GetSpawnedDartPin() { return spawnedDartpin; }
	UFUNCTION(BlueprintCallable)
		int GetCurrentDartpinIndex() { return currentDartpinIndex; }
	UFUNCTION(BlueprintCallable)
		class UWidget_DartGame* GetDartGameWidget();

	// SetterFunction
	UFUNCTION(BlueprintCallable)
		void SetUpSocketNames(FName socketname);
	UFUNCTION(BlueprintCallable)
		void SetUpDirections(FName RightEndPosName, FName LeftEndPosName);
	UFUNCTION(BlueprintCallable)
		void SetUpDefaultDistance();
	UFUNCTION(BlueprintCallable)
		void SetCurrentDartpinIndex(int index) { currentDartpinIndex = index; }

	// Overlap Events
	UFUNCTION(BlueprintCallable)
		void StartGame();
	
	// SpawnAction
	UFUNCTION()
		void SpawnDartPin(FVector SpawnLocation3D, FVector SpawnScale3D);

	// ShootAction
	UFUNCTION(BlueprintCallable)
		void ShootDartPin(FVector StartPos, FVector EndPos);
	UFUNCTION(BlueprintCallable)
		void ShootDartPinWithPressedTime(FVector StartPos, FVector EndPos, float PressedTimeSecond);

	UFUNCTION()
		void ShootFinished();

	// ScoreInfo
	UFUNCTION(BlueprintCallable)
		int GetScore(FVector impactPoint);
	UFUNCTION(BlueprintCallable)
		int GetScoreFromAngle(float angle);

	// Set Blend TargetView
	UFUNCTION(BlueprintCallable)
		void SetBlendTargetRoundEndCam(float blendTime);
	UFUNCTION(BlueprintCallable)
		void SetBlendTargetMainCam(float blendTime);

	// Reset Round
	UFUNCTION(BlueprintCallable)
		void ResetRound();

	// Exit Game
	UFUNCTION(BlueprintCallable)
		void ExitFromDartGame();

	UFUNCTION()
		void EndBlendViewTarget();

	void DrawRandomRange(FVector currentMousePositionToWorldPos, FColor color, float drawTime, float PressedTimeDelta);
	void DrawDebugPointOriginAndOffsetPoint(FVector OriginPoint, float drawTime);


public:
	// Delegate
	UPROPERTY(BlueprintAssignable)
		FSetScoreDelegate onScoredDelegate;
	UPROPERTY(BlueprintAssignable)
		FVoidDelVoidDYN onEndOfBlendViewTargetDelegate;

	// Distances
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", meta = (AllowPrivateAccess = true))
		float distBullsEye;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", meta = (AllowPrivateAccess = true))
		float distOuterBullsEye;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", meta = (AllowPrivateAccess = true))
		float distTrebleStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", meta = (AllowPrivateAccess = true))
		float distTrebleEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", meta = (AllowPrivateAccess = true))
		float distDoubleStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", meta = (AllowPrivateAccess = true))
		float distDoubleEnd;

	/** if Player Pressed Button in 0 sec when shoot Dartpin, return this Value by Default to calculate Random radius Range.  **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomRange")
		float maxRangeOfRandomRadius = 6.f;
	/** RandomRadius = maxRangeOfRandomRadius - (multipleValueOfPreessedTime * PressedTimeDelta)**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomRange")
		float multipleValueOfPressedTime = 2.f;
	
	/** OffsetZValue = PressedTimeDelta^2 * multipleOffsetZValue / divideOffsetZValue **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomRange")
		float multipleOffsetZValue = 8.0f;
	/** if this value lower than 1.0, will be setted 1.0. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomRange")
		float divideOffsetZValue = 9.0f;
	/** Set This Value For Game Round Time. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundTime")
		float DefaultRoundTime = 60.0f;



	// DartGame Widget Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UWidget_DartGame> DartGameWidgetClass;

	// DartPinActorClass
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DartPinClass)
		TSubclassOf<ADartPin> DartPinActorClass;

protected:
	UFUNCTION()
		void UpdateDartPinLocation(FVector newLocation, bool bTeleportPosition);
	UFUNCTION(BlueprintCallable)
		FVector2D GetRandomRangeOffset(float PressedTimeSec);

private:
	FName GetSocketNameByIndex(int index);
	EDART_SOCKET_NAMES GetSocketEnumByName(FName name);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* mesh;

	// FVector Variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Position", meta = (AllowPrivateAccess = true))
		FVector centerPos;

	// Collisions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
		UBoxComponent* boxCollision;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
		UBoxComponent* gameStartTrigger;

	// Dummy Info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = true))
		USphereComponent* DartBoardInfo;

	// Socket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Names", meta = (AllowPrivateAccess = true))
		TArray<FName> SocketNames;

	// Camera Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		UCameraComponent* camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		UCameraComponent* roundEndCamera;
	UPROPERTY()
		TWeakObjectPtr<ACameraActor> roundEndSpawnedCam;

	// DartPin SpawnLocation SceneComp
	UPROPERTY(EditAnywhere)
		USceneComponent* DartpinSpawnLocation;

	// Widget
	UPROPERTY()
		class UWidget_DartGame* dartGameWidget;

	// DartPin
	UPROPERTY()
		ADartPin* spawnedDartpin;
	UPROPERTY()
		TArray<ADartPin*> spawnedDartPinsArr;

	// Get Dartpin Index
	int currentDartpinIndex = 0;

	// TimerHandle
	FTimerHandle viewTargetHandle;
};


/*
	Widget 생성
		-> DartPin 생성
		-> DartPin 컨트롤

*/