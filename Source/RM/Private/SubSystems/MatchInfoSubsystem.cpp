// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystems/MatchInfoSubsystem.h"
#include "../RMProtocolFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "HttpModule.h"
#include "IImageWrapperModule.h"
#include "Engine/RendererSettings.h"
#include "Misc/DateTime.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Runtime/Online/HTTP/Public/Http.h"


void FMatchInfo::FromJson(const FString& JsonString)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, this);
}

void FMatchInfo::FromObject(TSharedPtr<FJsonObject> JsonObject)
{
	FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), this);
}



void UMatchInfoSubsystem::GetMatchInfo(const FMatchInfoErrorDelegate& InError,
	const FMatchInfoResponseDelegate& InResponse, FString Clubcode)
{
	OnError = InError;
	OnResponse = InResponse;

	FString API = URMProtocolFunctionLibrary::GetBaseUrl()+"/press/"+Clubcode;
	//
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = (&FHttpModule::Get())->CreateRequest();
	
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(API);
	HttpRequest->SetHeader("content-type", "application/json");
	HttpRequest->SetHeader("authorization", *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully)
	{
		if (connectedSuccessfully)
		{
			if (pResponse == nullptr)
			{
				OnError.ExecuteIfBound(0);
				return;
			}

			int32 Code = pResponse->GetResponseCode();
			if (Code != 200)
			{
				OnError.ExecuteIfBound(Code);
				return;
			}
			if (Code == 200)
			{
				FString JsonResponse = pResponse->GetContentAsString();

				TSharedPtr<FJsonObject> JsonParsed;
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonResponse);
				if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
				{
					FString ResultCode = JsonParsed->GetStringField("resultCode");
					FString SessionId  = JsonParsed->GetStringField("sessionId");
					TSharedPtr<FJsonObject> Data = JsonParsed->GetObjectField("data");
					MatchInfo.FromObject(Data);

					OnResponse.ExecuteIfBound(JsonResponse);
			}
		}
			else
			{
				OnError.ExecuteIfBound(Code);
			}
	}
		else
		{
			OnError.ExecuteIfBound(500);
		}
	});
	HttpRequest->ProcessRequest();
}

void UMatchInfoSubsystem::ConvertTimeStampToDateTime(int64 TimeStamp, FString& OutDate, FString& OutTime, FDateTime& OutDateTime)
{
	FDateTime DateTime = FDateTime::FromUnixTimestamp(TimeStamp/1000);
	OutDateTime = DateTime;
	OutDate = FString::Printf(TEXT("%d.%d.%d"), DateTime.GetYear(), DateTime.GetMonth(), DateTime.GetDay());
	OutTime = FString::Printf(TEXT("%d:%d"), DateTime.GetHour(), DateTime.GetMinute());
}
