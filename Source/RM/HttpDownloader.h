// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "HttpDownloader.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHttpDownloadStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHttpDownloadProcessDelegate, int32, RecvSize, int32, TotalSize, float, Percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHttpDownloadFinishedDelegate, bool, bIsSuccess, FString, ContentPath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHttpJsonDownloadFinishedDelegate, FString, ContentString);

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class RM_API UHttpDownloader : public UObject
{
	GENERATED_BODY()

public:
	UHttpDownloader();

	UFUNCTION(BlueprintCallable, DisplayName = "ExecuteDownload", Category = "HttpDownloader")
		void ExecuteDownload(FString sSourceURL, FString sPath, FString sName);

	UPROPERTY(BlueprintAssignable)
		FHttpDownloadStartDelegate OnDownloadStartCallback;

	UPROPERTY(BlueprintAssignable)
		FHttpDownloadProcessDelegate OnDownloadProcessCallback;

	UPROPERTY(BlueprintAssignable)
		FHttpDownloadFinishedDelegate OnDownloadFinishedCallback;

	UPROPERTY(BlueprintAssignable)
		FHttpJsonDownloadFinishedDelegate OnJsonDownloadFinishedCallback;

private:
	FHttpModule* HttpModule;

	void HttpRequestProgressDelegate(FHttpRequestPtr RequestPtr, int32 SendBytes, int32 RecvBytes);
	
	void HttpRequestFinishedDelegate(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	
	UPROPERTY()
		FString SourceURL;

	// Ex: /Folder/Packs/
	UPROPERTY()
		FString Path;
	// Ex: MyVideo.mp4
	UPROPERTY()
		FString Name;
};
