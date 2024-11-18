// Fill out your copyright notice in the Description page of Project Settings.


#include "RMBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "PatchSaveGame.h"
#include "Kismet/KismetStringLibrary.h"
#if PLATFORM_IOS
#include "IOSRuntimeSettings.h"
#elif PLATFORM_ANDROID
#include "AndroidRuntimeSettings.h"
#endif
#include "BaseRMGameInstance.h"
#include "Windows/LiveCodingServer/Public/ILiveCodingServer.h"
#include "ImageUtils.h"


FString URMBlueprintFunctionLibrary::ContentURL = TEXT("");
FConfigInfo URMBlueprintFunctionLibrary::ConfigInfo = {};


void URMBlueprintFunctionLibrary::SetConfig(FConfigInfo NewConfig)
{
	ConfigInfo = NewConfig;
}

FConfigInfo& URMBlueprintFunctionLibrary::GetConfigInfo()
{
	return ConfigInfo;
}

FString URMBlueprintFunctionLibrary::GetVersion()
{
	FString version;
#if PLATFORM_IOS
	version = GetMutableDefault<UIOSRuntimeSettings>()->VersionInfo;
#elif PLATFORM_ANDROID
	version = GetMutableDefault<UAndroidRuntimeSettings>()->VersionDisplayName;
#else
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		version,
		GGameIni
	);
#endif

	return version;
}

const FConfigInfo& URMBlueprintFunctionLibrary::GetConfig()
{
	return ConfigInfo;
}

bool URMBlueprintFunctionLibrary::CheckUpdateApp()
{
#if PLATFORM_WINDOWS
	return true;
#endif

	FString version = GetVersion();
	if (ConfigInfo.Version.Compare(version) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool URMBlueprintFunctionLibrary::CheckPatch()
{
#if PLATFORM_WINDOWS
	return true;
#endif

	UPatchSaveGame* LoadGameInstance = Cast<UPatchSaveGame>(UGameplayStatics::CreateSaveGameObject(UPatchSaveGame::StaticClass()));

	if (LoadGameInstance)
	{
		LoadGameInstance->SaveSlotName = "PatchData";
		LoadGameInstance->SaveIndex = 0;

		LoadGameInstance = Cast<UPatchSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->SaveIndex));

		if (LoadGameInstance && ConfigInfo.Revision == LoadGameInstance->Revision)
		{
			ConfigInfo.InstallDirs = LoadGameInstance->InstallDirs;
			return true;
		}	
	}

	return false;
}

void URMBlueprintFunctionLibrary::CompletePatch()
{
#if PLATFORM_WINDOWS
	return;
#endif

	UPatchSaveGame* SaveGameInstance = Cast<UPatchSaveGame>(UGameplayStatics::CreateSaveGameObject(UPatchSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->SaveSlotName = "Config";
		SaveGameInstance->SaveIndex = 0;

		SaveGameInstance->Revision = ConfigInfo.Revision;
		SaveGameInstance->InstallDirs = ConfigInfo.InstallDirs;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->SaveIndex);
	}
}

void URMBlueprintFunctionLibrary::MyQuitGame(bool Force)
{
	FPlatformMisc::RequestExit(Force);
}

/* @return MON = 0, TUE = 1, WED = 2, THU = 3, FRI = 4, SAT = 5, SUN = 6 */
int URMBlueprintFunctionLibrary::GetDayOfWeek(FDateTime dateTime)
{
	return (int)dateTime.GetDayOfWeek();
}

/* @return All Days of dateTime.Month */
int URMBlueprintFunctionLibrary::GetDayOfMonth(FDateTime dateTime)
{
	return dateTime.DaysInMonth(dateTime.GetYear(), dateTime.GetMonth());
}

FDateTime URMBlueprintFunctionLibrary::GetNextWeekFirstDay(FDateTime dateTime)
{
	int day = GetDayOfWeek(dateTime);
	FTimespan PlusDay = FTimespan(7 - day, 0, 0, 0, 0);
	FTimespan exceededTime = FTimespan(0, dateTime.GetHour(), dateTime.GetMinute(), dateTime.GetSecond(), dateTime.GetMillisecond());

	//dateTime += PlusDay;
	//dateTime -= exceededTime;
	return dateTime + (PlusDay - exceededTime);
}

FDateTime URMBlueprintFunctionLibrary::GetNextMonthFirstDay(FDateTime dateTime)
{
	FDateTime firstDayInMonth = FDateTime(dateTime.GetYear(), dateTime.GetMonth(), 1, 0, 0, 0, 0);
	return firstDayInMonth + FTimespan(GetDayOfMonth(dateTime),0,0,0);
}

FDateTime URMBlueprintFunctionLibrary::GetNextYearFirstDay(FDateTime dateTime)
{
	FDateTime firstDayInYear = FDateTime(dateTime.GetYear(), 1, 1, 0, 0, 0, 0);
	return firstDayInYear + FTimespan(dateTime.DaysInYear(dateTime.GetYear()),0,0,0);
}


UBaseRMGameInstance* URMBlueprintFunctionLibrary::GetBaseRMGameInstance(UObject* Context)
{
	if (UBaseRMGameInstance* RM = Cast<UBaseRMGameInstance>(UGameplayStatics::GetGameInstance(Context)))
	{
		return RM;
	}
	
	if (IsValid(Context) && IsValid(Context->GetWorld()))
	{
		return Cast<UBaseRMGameInstance>(UGameplayStatics::GetGameInstance(Context->GetWorld()));
	}

	if (IsValid(GEngine) && IsValid(GEngine->GetWorld()))
	{
		return Cast<UBaseRMGameInstance>(UGameplayStatics::GetGameInstance(GEngine->GetWorld()));
	}

	return nullptr;
}

void URMBlueprintFunctionLibrary::CreateInvisibleWidget(UObject* WorldContextObject, int ZOrder)
{
	GetBaseRMGameInstance(WorldContextObject)->CreateInvisibleWidget(ZOrder);
}

void URMBlueprintFunctionLibrary::RemoveInvisibleWidget(UObject* WorldContextObject)
{
	GetBaseRMGameInstance(WorldContextObject)->RemoveInvisibleWidget();
}

FDateTime URMBlueprintFunctionLibrary::GetServerUTCTime(UObject* WorldContextObject)
{
	return GetBaseRMGameInstance(WorldContextObject)->GetServerUTCTime(WorldContextObject);
}

bool URMBlueprintFunctionLibrary::UnloadStreamLevelDynamic(ULevelStreamingDynamic* levelStreamDYN)
{
	
	// return levelStreamDYN->GetWorld()->RemoveStreamingLevel(levelStreamDYN);

	if (levelStreamDYN == nullptr || levelStreamDYN->GetIsRequestingUnloadAndRemoval())
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelStreamingDynamic Level is already setting removal state."));
		return false;
	}

	levelStreamDYN->SetIsRequestingUnloadAndRemoval(true);
	return true;
}

ULevelStreamingDynamic* URMBlueprintFunctionLibrary::FindLevelStreamDynamicByName(UObject* WorldContextObject, FString PackageName)
{
	UWorld* world = WorldContextObject->GetWorld();
	
	if (world)
	{
		auto TL = world->GetStreamingLevels();
		for (ULevelStreaming* level : TL)
		{
			auto streamName = FPaths::GetBaseFilename(level->PackageNameToLoad.ToString());
			UE_LOG(LogTemp, Log, TEXT("%s"), *streamName);

			if (streamName.Contains(PackageName))
				return Cast<ULevelStreamingDynamic>(level);
		}
			
	}

	return nullptr;
}

FVector URMBlueprintFunctionLibrary::GetBezierPosition(FVector Start, FVector End, FVector ControlPos1, FVector ControlPos2, float alpha)
{
	FVector ControlledPos[5];
	ControlledPos[0] = FMath::Lerp<FVector>(Start, ControlPos1, alpha);
	ControlledPos[1] = FMath::Lerp<FVector>(ControlPos1, ControlPos2, alpha);
	ControlledPos[2] = FMath::Lerp<FVector>(ControlPos2, End, alpha);
	ControlledPos[3] = FMath::Lerp<FVector>(ControlledPos[0], ControlledPos[1], alpha);
	ControlledPos[4] = FMath::Lerp<FVector>(ControlledPos[1], ControlledPos[2], alpha);

	return FMath::Lerp<FVector>(ControlledPos[3], ControlledPos[4], alpha);
}

float URMBlueprintFunctionLibrary::FCubicInterpEase(float Start, float End, float alpha)
{
	if (Start > End)
		Swap<float>(Start, End);

	alpha = FMath::Clamp<float>(alpha, 0.0f, 1.0f);
	float returnValue = FMath::CubicInterp<float>(Start, 1.0f, End, 1.0f, alpha);
	UE_LOG(LogTemp, Log, TEXT("CubivInterp :%.2f"), returnValue);
	return returnValue;
}


bool URMBlueprintFunctionLibrary::IsForbidden(FString InputString, FString RegexString, FString& ModifiedString)
{
	FString cpyString = InputString.ToUpper();
	FRegexPattern patt(RegexString.ToUpper());
	FRegexMatcher matcher(patt, cpyString);

	bool bIsMatched = false;
	while (matcher.FindNext())
	{
		int begin = matcher.GetMatchBeginning();
		int end = matcher.GetMatchEnding();
		FString word = UKismetStringLibrary::GetSubstring(InputString, begin, end - begin);
		FString modified = TEXT("");
		for (int i = 0; i < word.Len(); i++)
			modified += TEXT("*");

		InputString.RemoveAt(begin, end - begin);
		InputString.InsertAt(begin, modified);
		cpyString.RemoveAt(begin, end - begin);
		cpyString.InsertAt(begin, modified);

		matcher = FRegexMatcher(patt, cpyString);
		bIsMatched = true;
		UE_LOG(LogTemp, Warning, TEXT("InputString = %s"), *InputString);
	}

	ModifiedString = InputString;
	return bIsMatched;
}

EBuildType URMBlueprintFunctionLibrary::GetCurrentBuildType()
{
#if UE_EDITOR
	return EBuildType::EDITOR;
#elif UE_BUILD_DEBUG
	return EBuildType::DEBUG;
#elif UE_BUILD_DEVELOPMENT
	return EBuildType::DEVELOPMENT;
#elif UE_BUILD_SHIPPING
	return EBuildType::SHIPPING;
#endif
	return EBuildType::EDITOR;
}

#pragma region Fade In/Out
void URMBlueprintFunctionLibrary::FillScreen(UObject* WorldContextObject, FLinearColor Color)
{
	const UWorld* world = WorldContextObject->GetWorld();
	if (world) {
		URMGameViewportClient* GameViewportClient = Cast<URMGameViewportClient>(world->GetGameViewport());
		if (GameViewportClient)	{
			GameViewportClient->Fill(Color);
		}
	}
}

void URMBlueprintFunctionLibrary::ScreenFadeIn(UObject* WorldContextObject, float Duration, FLinearColor Color,const FNotifyGameViewportChanged& OnNotifyGameViewportChanged)
{
	const UWorld* world = WorldContextObject->GetWorld();
	if (world) {
		URMGameViewportClient* GameViewportClient = Cast<URMGameViewportClient>(world->GetGameViewport());
		if (GameViewportClient)	{
			GameViewportClient->FadeIn(Duration, Color, OnNotifyGameViewportChanged);
		}
	}
}

void URMBlueprintFunctionLibrary::ScreenFadeOut(UObject* WorldContextObject, float Duration, FLinearColor Color, const FNotifyGameViewportChanged& OnNotifyGameViewportChanged)
{
	const UWorld* world = WorldContextObject->GetWorld();
	if (world) {
		URMGameViewportClient* GameViewportClient = Cast<URMGameViewportClient>(world->GetGameViewport());
		if (GameViewportClient)	{
			GameViewportClient->FadeOut(Duration, Color, OnNotifyGameViewportChanged);
		}
	}
}
#pragma endregion

void URMBlueprintFunctionLibrary::DisableWorldRendering(UObject* WorldContextObject, bool bDisable) {
	const UWorld* world = WorldContextObject->GetWorld();
	if (world) {
		URMGameViewportClient* GameViewportClient = Cast<URMGameViewportClient>(world->GetGameViewport());
		if (GameViewportClient) {
			GameViewportClient->bDisableWorldRendering = bDisable;
		}
	}
}

int URMBlueprintFunctionLibrary::GetStringByteSizeToUTF8(FString InString)
{
	FTCHARToUTF8 ToUtf8Conv(InString.GetCharArray().GetData());
	return ToUtf8Conv.Length();
}

FString URMBlueprintFunctionLibrary::ConvertStringDesiredByteSize(FString InString, const int desiredSize)
{
	FString cpyString = InString;
	while (GetStringByteSizeToUTF8(cpyString) > desiredSize)
		cpyString = cpyString.LeftChop(1);
	
	return cpyString;
}

UTexture2D* URMBlueprintFunctionLibrary::CreateBitTextureAtRuntime(TArray<uint8> BGRA8PixelData)
{
#define UpdateResource UpdateResource
	UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(BGRA8PixelData);
	Texture->UpdateResource();
	return Texture;
}