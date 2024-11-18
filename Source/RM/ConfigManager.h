#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "ConfigStruct.h"

#if PLATFORM_WINDOWS
#include "AndroidRuntimeSettings.h"
#include "IOSRuntimeSettings.h"
#elif PLATFORM_ANDROID
#include "AndroidRuntimeSettings.h"
#elif PLATFORM_IOS
#include "IOSRuntimeSettings.h"
#endif

#include "ConfigManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHttpConfigDownloadFinishedDelegate, bool, bIsSuccess, FString, ContentString);

UENUM(BlueprintType)
enum class EConfigType : uint8
{
	E_Andorid UMETA(DisplayName = "Android"),
	E_iOS UMETA(DisplayName = "iOS"),
};

/**
 *
 */

UCLASS(Blueprintable, BlueprintType)
class RM_API UConfigManager : public UObject
{
	GENERATED_BODY()

public:
	UConfigManager();

	UPROPERTY(BlueprintAssignable)
		FHttpConfigDownloadFinishedDelegate OnDownloadCompleteDelegates;

	UFUNCTION(BlueprintCallable, DisplayName = "ExecuteDownload", Category = "ConfigManager")
		void ExecuteDownload(FString sSourceURL);


	UFUNCTION(BlueprintCallable, DisplayName = "VersionCheck", Category = "ConfigManager")
		bool VersionCheck(FConfigStruct config);

	UFUNCTION(BlueprintCallable, DisplayName = "FileLoad", Category = "ConfigManager")
		FString FileLoad(FString Path);

	UFUNCTION(BlueprintCallable, DisplayName = "FileParsing", Category = "ConfigManager")
		FConfigStruct FileParsing(FString PlatformString, FString FullPath, EConfigType DefaultType);

	UFUNCTION(BlueprintCallable, DisplayName = "HttpParsing", Category = "ConfigManager")
		FConfigStruct HttpParsing(FString PlatformString, EConfigType DefaultType);

	UFUNCTION(BlueprintCallable, DisplayName = "ManifestArrayInfo", Category = "ConfigManager")
		void ManifestArrayInfo(TArray<FString>& ArrayManifestURL, FString& CloudURL, FString& IntallDirectory);

	UFUNCTION(BlueprintCallable, DisplayName = "GetState", Category = "ConfigManager")
		int32 GetState();

	UFUNCTION(BlueprintCallable, DisplayName = "IsRequestCompleted", Category = "ConfigManager")
		bool IsRequestCompleted();

	UFUNCTION(BlueprintCallable, DisplayName = "GetVersionString", Category = "ConfigManager")
		FString GetVersionString();

private:
	FHttpModule* HttpModule;
	FString ContentString;
	FConfigStruct ConfigStruct;
	EConfigType ConfigType;
	int32 State;

	void HttpRequestFinishedDelegate(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void GenerateStructsFromJson(FConfigStruct& ConfigStructs, TSharedPtr<FJsonObject> JsonObject);
};
