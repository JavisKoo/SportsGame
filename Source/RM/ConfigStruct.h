// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ConfigStruct.generated.h"

/**
 * 
 {
	"Version": {
		"Major": "1",
		"Minor": "0",
		"Revision": "0"
	},
	"ServerState": 1,
	"ServerIP": "127.0.0.1",
	"ServerPort": 80,
	"InstallDir": "DLContent",
	"ManifestDir": "http://3dpot.tv/data/Patch/Test/Android_ETC2/ManifestDir/",
	"CloudURL": "http://3dpot.tv/data/Patch/Test/Android_ETC2/CloudDir/",
	"ManifestArray":
	[
		"MySamples_pakchunk1release1.manifest",
		"MySamples_pakchunk2release2.manifest",
		"MySamples_pakchunk3release3.manifest"
	]
}
 */
USTRUCT(Blueprintable, BlueprintType)
struct FConfigStruct
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		int32	Major;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		int32	Minor;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		int32	Revision;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
        int32	ServerState;
    UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
        FString ServerIP;
    UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		int32	ServerPort;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		FString InstallDir;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		FString ManifestDir;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		FString CloudURL;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "ConfigStruct")
		TArray<FString> ManifestArray;

public:
    static FConfigStruct BuildConfigStruct(
		int32	Major,
		int32	Minor,
		int32	Revision,
		int32	ServerState,
		FString ServerIP,
		int32	ServerPort,
		FString InstallDir,
		FString ManifestDir,
		FString CloudURL,
		TArray<FString> ManifestArray
    );
};
