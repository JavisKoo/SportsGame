// Fill out your copyright notice in the Description page of Project Settings.


#include "RMProtocolFunctionLibrary.h"
#include "Http.h"

DEFINE_LOG_CATEGORY(LogRMProtocol)

FString URMProtocolFunctionLibrary::_BaseUrl = TEXT("https://fcb.3df.co.kr");//dis server
//FString URMProtocolFunctionLibrary::_BaseUrl = TEXT("https://dev-fcb.3df.co.kr");//dev server
FString URMProtocolFunctionLibrary::_Token = TEXT("");
int32 URMProtocolFunctionLibrary::_UserSeq = 0;
FString URMProtocolFunctionLibrary::_EncryptionUserSeq = TEXT("");
bool URMProtocolFunctionLibrary::_IsBackdoor = false;
FOnMultipleAccessError URMProtocolFunctionLibrary::OnMultipleAccessErrorDelegate;
FOnCheckServer URMProtocolFunctionLibrary::OnCheckServerDelegate;

void URMProtocolFunctionLibrary::SetBaseUrl(const FString& url)
{
	_BaseUrl = url;
}

const FString URMProtocolFunctionLibrary::GetBaseUrl()
{
	return _BaseUrl;
}

void URMProtocolFunctionLibrary::SetAccessToken(const FString& token)
{
	_Token = token;
}

const FString URMProtocolFunctionLibrary::GetAccessToken()
{
	return _Token;
}

FString URMProtocolFunctionLibrary::GetProtocolErrorMessage(const FHttpResponsePtr Response, UClass* classType)
{
	FString msg = FString::Printf(TEXT("Error processing request.\nProtocol: %s\nError Code: %d"),
		*(classType->GetName()), Response->GetResponseCode());

	return msg;
}

bool URMProtocolFunctionLibrary::CheckResponse(const FHttpResponsePtr Response, bool WasSuccessful, UClass* ClassType, int32& ErrorCode, FString& ErrorMsg)
{
	if (Response == nullptr)
	{
		ErrorCode = 0;
		ErrorMsg = TEXT("The Internet connection appears to be offline.");
		return false;
	}

	ErrorCode = Response->GetResponseCode();
	if (!WasSuccessful || ErrorCode != 200)
	{
		ErrorMsg = FString::Printf(TEXT("Error processing request.\nProtocol: %s\nError Code: %d"),
			*(ClassType->GetName()), ErrorCode);
		return false;
	}
	
	return true;
}


void URMProtocolFunctionLibrary::SetUserSeq(int32 UserSeq)
{
	_UserSeq = UserSeq;
}

void URMProtocolFunctionLibrary::SetEncryptionUserSeq(FString encSeq)
{
	_EncryptionUserSeq = encSeq;
}

int32 URMProtocolFunctionLibrary::GetUserSeq()
{
	return _UserSeq;
}

FString URMProtocolFunctionLibrary::GetEncryptionUserSequence()
{
	return _EncryptionUserSeq;
}

bool URMProtocolFunctionLibrary::CheckSubErrorCode(TSharedPtr<FJsonObject> json)
{
	int resultCode = json->GetIntegerField("resultCode");
	
	if (resultCode == 4001)	// �ߺ� �α���
	{
		URMProtocolFunctionLibrary::OnMultipleAccessErrorDelegate.Broadcast();
	}
	else if (resultCode == 503)	// ���� ����
	{
		TSharedPtr<FJsonObject> data = json->GetObjectField(TEXT("data"));
		FDateTime WorkoutTime;
		FDateTime::Parse(data->GetStringField("workoutTime"), WorkoutTime);

		URMProtocolFunctionLibrary::OnCheckServerDelegate.Broadcast(WorkoutTime);
	}
	else
	{
		return false;
	}

	return true;
}

void URMProtocolFunctionLibrary::SetBackdoor(bool IsBackdoor)
{
	_IsBackdoor = IsBackdoor;
}

void URMProtocolFunctionLibrary::SetDefaultHeader(FHttpRequestPtr HttpRequest)
{
	HttpRequest->SetHeader(TEXT("content-type"), TEXT("application/json"));
	if (_IsBackdoor)
		HttpRequest->SetHeader(TEXT("Range"), TEXT("develop"));
}

FHttpRequestPtr URMProtocolFunctionLibrary::GetDefaultHttp(const FString& URL, const FString& Method)
{
	auto HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *_BaseUrl, *URL));

	HttpRequest->SetHeader(TEXT("content-type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("authorization"), *_Token);
	if (_IsBackdoor)
		HttpRequest->SetHeader(TEXT("Range"), TEXT("develop"));

	HttpRequest->SetVerb(Method);

	return HttpRequest;
}

void URMProtocolFunctionLibrary::ProcessDefaultHttp(FHttpRequestPtr HttpRequest,
	FOnHttpRequestResponseDelegate& OnResponse, FOnHttpRequestErrorDelegate& OnError,
	TFunction<void(TSharedPtr<FJsonObject>)> SucceededFunction)
{
	if (HttpRequest->ProcessRequest())
	{
		UE_LOG(LogRMProtocol, Log, TEXT("RequestURL : %s"), *HttpRequest->GetURL());
		HttpRequest->OnProcessRequestComplete().BindLambda([&, SucceededFunction](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccess)
		{	// START LAMBDA
			CHECK_HTTP_RESPONSE(Response, FOnHttpRequestErrorDelegate, OnError, Response->GetResponseCode());
			UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
			TSharedPtr<FJsonObject> res;
			TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(reader, res))
			{
				const int resultCode = res->GetNumberField("resultCode");
				if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
				{
					EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestErrorDelegate, OnError, resultCode);
					return;
				}
				else if (resultCode == 200)
				{
					SucceededFunction(res);
					return;
				}
				else
				{
					UE_LOG(LogRMProtocol, Log, TEXT("RequestURL : %s  -  ResultCode = %d"), *Request->GetURL(), resultCode);
					EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestErrorDelegate, OnError, resultCode);
					return;
				}
			}
		});	// END LAMBDA
	}
	else
	{
		UE_LOG(LogRMProtocol, Log, TEXT("%s - Error Process"), *HttpRequest->GetURL());
		EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestErrorDelegate, OnError, 0);
	}
}
