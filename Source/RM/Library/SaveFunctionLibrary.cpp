// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveFunctionLibrary.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"

USaveFunctionLibrary::USaveFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USaveFunctionLibrary::SavePlayerUserDataJson(const int32 PlayerPieceCount, const bool bAcquisition, const bool bInPlayer, const FString& JsonPath)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FPlayerUserData(PlayerPieceCount, bAcquisition, bInPlayer), JsonString);
	FFileHelper::SaveStringToFile(*JsonString, *JsonPath);
}

FPlayerUserData USaveFunctionLibrary::GetPlayerUserDataByJson(const FString& JsonPath)
{
	FPlayerUserData PlayerUserData;
	FString JsonString;

	if (FPaths::FileExists(*JsonPath))
	{
		FFileHelper::LoadFileToString(JsonString, *JsonPath);

		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			PlayerUserData.PlayerPieceCount = JsonObject->GetIntegerField(TEXT("PlayerPieceCount"));
			PlayerUserData.bAcquisition = JsonObject->GetBoolField(TEXT("bAcquisition"));
			PlayerUserData.bInPlayer = JsonObject->GetBoolField(TEXT("bInPlayer"));
		}
	}
	else
	{
		FJsonObjectConverter::UStructToJsonObjectString(FPlayerUserData(), JsonString);
		FFileHelper::SaveStringToFile(*JsonString, *JsonPath);		
	}

	return PlayerUserData;
}
