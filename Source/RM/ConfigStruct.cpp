// Fill out your copyright notice in the Description page of Project Settings.
#include "ConfigStruct.h"

FConfigStruct FConfigStruct::BuildConfigStruct(
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
)
{
	FConfigStruct config;

	config.Major = Major,
	config.Minor = Minor,
	config.Revision = Revision, 
	config.ServerState = ServerState;
	config.ServerIP = ServerIP;
	config.ServerPort = ServerPort;
	config.InstallDir = InstallDir;
	config.ManifestDir = ManifestDir;
	config.CloudURL = CloudURL;
	config.ManifestArray = ManifestArray;
	
	return config;
}
