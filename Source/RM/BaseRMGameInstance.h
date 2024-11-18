// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "Blueprint/UserWidget.h"
#include "HttpModule.h"
#include "BaseRMGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UBaseRMGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
		void OnMultipleAccessError();
	virtual void OnMultipleAccessError_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void OnCheckServer(FDateTime WorkoutTime);
	virtual void OnCheckServer_Implementation(FDateTime WorkoutTime);

	UFUNCTION(BlueprintCallable)
		void BindMultipleAccessError();

	UFUNCTION(BlueprintCallable)
		void BindCheckServer();



	UFUNCTION(BlueprintCallable)
		void AddToViewportClientContents(UUserWidget* WidgetObject);
	UFUNCTION(BlueprintCallable)
		void RemoveFromViewportClientContents(UUserWidget* WidgetObject);

	UFUNCTION(BlueprintCallable)
		void CreateInvisibleWidget(int ZOrder);

	UFUNCTION(BlueprintCallable)
		void RemoveInvisibleWidget();

	/* Setting ServerUTCTime by param serverTime. you call this function one time in your gameplay.
	*  [!] Attention: to use this function, should call after setting userinfo::ServerTime by backend Server. */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Time|Server Time")
		void InitServerUTCTime(const UObject* WorldContextObject, FDateTime serverTime);
	/* Get UTC Time From APP Opened
	*  [!] Attention: to use this function, should call after setting userinfo::ServerTime by backend Server. */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Time|Server Time")
		FDateTime GetServerUTCTime(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void TestLog(FString Log);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void TestLog2(FString UserID, FString Log);

	UFUNCTION(BlueprintCallable)
		void PlaySFXSound3D(class USoundBase* sound, FVector location, FRotator rotation, float volumeMultiplier = 1.0f, float pitchMultiplier = 1.0f, float startTime = 0.0f, class USoundAttenuation* attenuation = nullptr, class USoundConcurrency* concurrency = nullptr, AActor* owner = nullptr);

	void Response_TestLog(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<UUserWidget> invisibleWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
		bool bIsSFXPlay = true;

	UPROPERTY(BlueprintReadWrite, Category = PlayerCollection)
	int32 CurrentPlayerCollectionPlayerId = INDEX_NONE;

protected:
	/* Reset ServerUTCTime. This Function must call restart Game. */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Time|Server Time")
	void ResetServerUTCTime();

private:
	/* UTC Time Seconds */
	bool bIsServerUTCSetting = false;
	FDateTime ServerUTCTime;
	float offsetToServerTime = 0.0f;
	float prevPlayTime = 0.0f;

};
