// Fill out your copyright notice in the Description page of Project Settings.


#include "Stream/CDNSubsystem.h"
#include "../RMProtocolFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "HttpModule.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Runtime/Online/HTTP/Public/Http.h"


void FDATA::FromJson(const FString& JsonString)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, this);
}

void FDATA::FromObject(TSharedPtr<FJsonObject> JsonObject)
{
	FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), this);
}

FString FVideoLikes::ToJson() {
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(*this, JsonString);
	return JsonString;
}


void FVideoLikeResponse::FromObject(TSharedPtr<FJsonObject> JsonObject)
{{
	FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), this);
}
}


void UCDNSubsystem::Refresh()
{
	if (OnRefresh.IsBound())
		OnRefresh.Broadcast();
}


void UCDNSubsystem::GetCDN(const FCDNErrorDelegate& InError, const FCDNResponseDelegate& InResponse, FString Category, const FString LanguageCode, FString Page, FString Limit)
{
	OnError = InError;
	OnResponse = InResponse;

	FString API = URMProtocolFunctionLibrary::GetBaseUrl()+"/qatar-video/list/"+Category+"?page="+Page+"&limit="+Limit;
	//const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("/qatar-video/list")+Category+"?page="+Page+"&limit="+Limit,TEXT("GET"));
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = (&FHttpModule::Get())->CreateRequest();
	
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(API);
	HttpRequest->SetHeader("content-type", "application/json");
	HttpRequest->SetHeader("content-language", LanguageCode);
	HttpRequest->SetHeader("authorization", *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully)
	{
		if (connectedSuccessfully)
		{
			if (pResponse==nullptr)
			{
				OnError.ExecuteIfBound(0);
				return;
			}
			
			int32 Code = pResponse->GetResponseCode();
			if (Code==200)
			{
				FString JsonResponse = pResponse->GetContentAsString();

				TSharedPtr<FJsonObject> JsonParsed;
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonResponse);
				if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
				{
					FString ResultCode = JsonParsed->GetStringField("resultCode");
					FString SessionId  = JsonParsed->GetStringField("sessionId");
					TSharedPtr<FJsonObject> Data = JsonParsed->GetObjectField("data");
					CDNData.FromObject(Data);
						
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

void UCDNSubsystem::PostLikes(const FPOSTErrorDelegate& InError, const FPOSTResponseDelegate& InResponse, int Seq)
{
	OnErrorPost = InError;
	OnResponsePost = InResponse;

	//FString API = URMProtocolFunctionLibrary::GetBaseUrl()+"/qatar-video/like";

	//TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = (&FHttpModule::Get())->CreateRequest();
	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("qatar-video/like"),TEXT("POST"));
	//HttpRequest->SetVerb("POST");
	//HttpRequest->SetURL(API);
	//HttpRequest->SetHeader("content-type", "application/json");
	//HttpRequest->SetHeader("authorization", "Bearer" + URMProtocolFunctionLibrary::GetAccessToken());
	
	
	FVideoLikes Body;
	//get seq in FCDN
	Body.Seq = Seq;
	//int to string

	HttpRequest->SetContentAsString(Body.ToJson());
	
	HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully)
	{
		if (connectedSuccessfully)
		{
			if (pResponse==nullptr)
			{
				OnErrorPost.ExecuteIfBound(0);
				return;
			}
			
			int32 Code = pResponse->GetResponseCode();
			if (Code==200)
			{
				//여기서 서명 오류 뜸 4002
				FString JsonResponse = pResponse->GetContentAsString();

				TSharedPtr<FJsonObject> JsonParsed;
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonResponse);
				if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
				{
					FString ResultCode = JsonParsed->GetStringField("resultCode");
					FString SessionId  = JsonParsed->GetStringField("sessionId");
					TSharedPtr<FJsonObject> Data = JsonParsed->GetObjectField("data");
					VideoLikeResponse.FromObject(Data);

					
					OnResponsePost.ExecuteIfBound(JsonResponse);
				}
				
			}
			else
			{
				OnErrorPost.ExecuteIfBound(Code);
			}
		}
		else
		{
			OnErrorPost.ExecuteIfBound(500);
		}
	});
	HttpRequest->ProcessRequest();
}
