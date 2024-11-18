// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Kismet/BlueprintFunctionLibrary.h"
#include "RMStruct.h"
//#include "BaseRMGameInstance.h"
#include "RMGameViewportClient.h"
#include "RMBlueprintFunctionLibrary.generated.h"


class UBaseRMGameInstance;
/**
 * 
 */

UENUM(BlueprintType)
enum class EBuildType : uint8
{
	EDITOR,
	DEBUG,
	DEVELOPMENT,
	SHIPPING,
};

UCLASS()
class RM_API URMBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SetConfig(FConfigInfo NewConfig);
	static FConfigInfo& GetConfigInfo();
	static FString GetVersion();

	UFUNCTION(BlueprintPure, Category = "PatchLibrary")
		static const FConfigInfo& GetConfig();

	UFUNCTION(BlueprintCallable, Category = "PatchLibrary")
		static bool CheckUpdateApp();

	UFUNCTION(BlueprintCallable, Category = "PatchLibrary")
		static bool CheckPatch();

	UFUNCTION(BlueprintCallable, Category = "PatchLibrary")
		static void CompletePatch();

	UFUNCTION(BlueprintCallable, Category = "Game")
		static void MyQuitGame(bool Force);

	/* GetDayOfWeek. return MON = 0 , TUE = 1, WED = 2 ...*/
	UFUNCTION(BlueprintPure, Category = "Time")
		static int GetDayOfWeek(FDateTime dateTime);

	/* Get Days Param's Month. */
	UFUNCTION(BlueprintPure, Category = "Time")
		static int GetDayOfMonth(FDateTime dateTime);

	/* Get Next Week's First Date. (2021.11.12 -> 2021.11.15 00:00) */
	UFUNCTION(BlueprintPure, Category = "Time")
		static FDateTime GetNextWeekFirstDay(FDateTime dateTime);

	/* Get Next Month's First Date. (2021.11.12 -> 2022.12.01, 00:00) */
	UFUNCTION(BlueprintPure, Category = "Time")
		static FDateTime GetNextMonthFirstDay(FDateTime dateTime);

	/* Get Next Year's First Date.(2021.11.12 -> 2022.01.01, 00:00) */
	UFUNCTION(BlueprintPure, Category = "Time")
		static FDateTime GetNextYearFirstDay(FDateTime dateTime);

	UFUNCTION(BlueprintPure, Category = "RM")
		static UBaseRMGameInstance* GetBaseRMGameInstance(UObject* Context);

	/* Widget */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Widget")
		static void CreateInvisibleWidget(UObject* WorldContextObject, int ZOrder);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Widget")
		static void RemoveInvisibleWidget(UObject* WorldContextObject);

	/* Get UTC Time From Server Time
	*  [!] Attention: to use this function, should call after setting userinfo::ServerTime by backend Server. */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Server | UTCTime")
		static FDateTime GetServerUTCTime(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RMLevelStreaming")
		static bool UnloadStreamLevelDynamic(class ULevelStreamingDynamic* levelStreamDYN);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RMLevelStreaming")
		static class ULevelStreamingDynamic* FindLevelStreamDynamicByName(UObject* WorldContextObject, FString PackageName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static FVector GetBezierPosition(FVector Start, FVector End, FVector ControlPos1, FVector ControlPos2, float alpha);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMBlueprintLibrary | Math")
		static float FCubicInterpEase(float Start, float End, float alpha);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMString | RegexMathcer")
		static bool IsForbidden(FString InputString, FString RegexString, FString& ModifiedString);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMBlueprintLibrary")
		static EBuildType GetCurrentBuildType();

	/* Get Converted String(UTF-8) Byte Size
	 * WhiteSpace = 1Byte, Eng = 1Byte, Spanish = 2Byte...
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMBlueprintLibrary")
	static int GetStringByteSizeToUTF8(FString InString);

	/* return String By Desired Byte Size (standard UTF-8) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMBlueprintLibrary")
	static FString ConvertStringDesiredByteSize(FString InString, const int desiredSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMBlueprintLibrary")
		static UTexture2D* CreateBitTextureAtRuntime(TArray<uint8> BGRA8PixelData);

#pragma region ScreenFader
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RMBlueprintLibrary|ScreenFader")
	static void FillScreen(UObject* WorldContextObject, FLinearColor Color);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RMBlueprintLibrary|ScreenFader")
	static void ScreenFadeIn(UObject* WorldContextObject, float Duration, FLinearColor Color, const FNotifyGameViewportChanged& OnNotifyGameViewportChanged);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RMBlueprintLibrary|ScreenFader")
	static void ScreenFadeOut(UObject* WorldContextObject, float Duration, FLinearColor Color, const FNotifyGameViewportChanged& OnNotifyGameViewportChanged);
#pragma endregion 

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "RMBlueprintLibrary|DisableWorldRendering")
	static void DisableWorldRendering(UObject* WorldContextObject, bool bDisable);

private:
	static FString ContentURL;
	static FConfigInfo ConfigInfo;
};
