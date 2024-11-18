// Fill out your copyright notice in the Description page of Project Settings.


#include "RMProtocol.h"
#include "Http.h"
#include "RMProtocolFunctionLibrary.h"
#include "RMStruct.h"
#include "RMBlueprintFunctionLibrary.h"
#include "RM_Singleton.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "RMEventSubSystem.h"

FString UServerInfoSetting::FirstUrl = TEXT("https://fcb.3df.co.kr");//dis Server
//FString UServerInfoSetting::FirstUrl = TEXT("https://dev-fcb.3df.co.kr");//dev Server
//--------------------------------------------------------------------------------------

UServerInfoSetting* UServerInfoSetting::SetServerInfo()
{
	UServerInfoSetting* BPNode = NewObject<UServerInfoSetting>();
	return BPNode;
}	

void UServerInfoSetting::Activate()
{
	FHttpModule::Get().SetHttpTimeout(30.0f);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/info/config"), *FirstUrl));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetVerb(TEXT("POST"));
	

	FString label;
#if PLATFORM_IOS
	label = TEXT("PR_apple");
#elif PLATFORM_ANDROID
	if (UKismetSystemLibrary::GetGameBundleId().Equals(TEXT("com.realmadrid.rmvw.huawei")))
		label = TEXT("huawei");
	else
		label = TEXT("google");
#else
	label = TEXT("google");
#endif

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("label", label);
	_payloadJson->SetStringField("version", URMBlueprintFunctionLibrary::GetVersion());

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UServerInfoSetting::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("SetServerInfo : %s, %s, %s"), *HttpRequest->GetURL(), *_payload, *HttpRequest->GetHeader(TEXT("Range")));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UServerInfoSetting - cannot process request"));
		Finished.Broadcast(0, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UServerInfoSetting::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FConfigInfo configInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}
	
	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), msg);
		SetReadyToDestroy();
		return;
	}

	int resultCode = Response->GetResponseCode();
	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}	

			resultCode = res->GetIntegerField("resultCode");
			TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

			URMProtocolFunctionLibrary::SetBaseUrl(data->GetStringField(TEXT("url")));
			GEngine->AddOnScreenDebugMessage(1, 10.0f, FColor::Green, FString(data->GetStringField(TEXT("url"))));
			configInfo.Version = URMBlueprintFunctionLibrary::GetVersion();

			configInfo.Revision = data->GetIntegerField(TEXT("revision"));
			configInfo.UpdateURL = data->GetStringField(TEXT("updateURL"));
			configInfo.ManifestURL = data->GetStringField(TEXT("manifestURL"));
			configInfo.CloudURL = data->GetStringField(TEXT("cloudURL"));
			configInfo.PhotonVersion = data->GetStringField(TEXT("photonVersion"));
			configInfo.PhotonProtocol = data->GetStringField(TEXT("protocol"));

			if (data->GetStringField(TEXT("state")).Compare(TEXT("on")) == 0)
			{
				resultCode = 7000;
				FDateTime::Parse(data->GetStringField("workoutTime"), configInfo.WorkoutTime);
			}
		}
	}

	URMBlueprintFunctionLibrary::SetConfig(configInfo);

	Finished.Broadcast(resultCode, TEXT(""));
	SetReadyToDestroy();
}


//--------------------------------------------------------------------------------------

ULoginAPI* ULoginAPI::Login(FString FirebaseToken, FString GoogleToken)
{
	ULoginAPI* BPNode = NewObject<ULoginAPI>();

	BPNode->_FirebaseToken = FirebaseToken;
	BPNode->_GoogleToken = GoogleToken;
	return BPNode;
}

void ULoginAPI::Activate()
{	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/signIdToken"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), FString::Printf(TEXT("Bearer %s"), *_FirebaseToken));
	HttpRequest->SetHeader(TEXT("Proxy-Authorization"), FString::Printf(TEXT("Bearer %s"),*_GoogleToken));
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("Login : %s, %s"), *HttpRequest->GetURL(), *_FirebaseToken);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("ULoginAPI - cannot process request"));
		Finished.Broadcast("", false, 0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void ULoginAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast("", false, 0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast("", false, Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
		
		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				
				URMProtocolFunctionLibrary::SetAccessToken(FString::Printf(TEXT("Bearer %s"), *data->GetStringField("accessToken")));
				
				Finished.Broadcast(data->GetStringField("uid"), data->GetBoolField("newbie"), resultCode, true, TEXT(""));
			}
			else
			{	
				// Finished.Broadcast(FString::FromInt(resultCode), false, resultCode, false, res->GetStringField("resultMessage"));
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				Finished.Broadcast(data->GetStringField("uid"), false, resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UETC_DataAPI* UETC_DataAPI::ETC_Data()
{
	UETC_DataAPI* BPNode = NewObject<UETC_DataAPI>();
	return BPNode;
}

void UETC_DataAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/info/etc-data"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader("content-type", "application/json");
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UETC_DataAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ETC-Data"));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("ULoginAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("etc-data request fail"), TEXT(""));
		SetReadyToDestroy();
	}
}

void UETC_DataAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), TEXT(""));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, TEXT(""));
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
		
		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				if (data->GetStringField("community_url")==TEXT("")==true)
				{
					Finished.Broadcast( resultCode, true, TEXT(""),TEXT("NotReady"));	
				}
				else
				{
				Finished.Broadcast( resultCode, true, TEXT(""),data->GetStringField("community_url"));
				}
			}
			else
			{	
				// Finished.Broadcast(FString::FromInt(resultCode), false, resultCode, false, res->GetStringField("resultMessage"));
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), TEXT(""));
			}
		}
	}
	SetReadyToDestroy();
}

// --------------------------------------------------------------------------------------------------------------------------------

#pragma region PARSE
ULoginWithParseAPI* ULoginWithParseAPI::LoginWithParse(ELoginWithParseType LoginWithParseType, FString ID_Token, FString UserID, FString SessionToken)
{
	ULoginWithParseAPI* BPNode = NewObject<ULoginWithParseAPI>();
	BPNode->LoginWithParseType = LoginWithParseType;
	BPNode->ID_Token = ID_Token;
	BPNode->UserID = UserID;
	BPNode->SessionToken = SessionToken;
	return BPNode;
}

void ULoginWithParseAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	
	switch (LoginWithParseType)
	{
		case ELoginWithParseType::Guest:
			HttpRequest->SetURL(FString::Printf(TEXT("%s/oauth/guest"), *URMProtocolFunctionLibrary::GetBaseUrl()));
			_payloadJson->SetStringField("username", *ID_Token);
			_payloadJson->SetStringField("password", *ID_Token);
			break;
		
		case ELoginWithParseType::Huawei:
			HttpRequest->SetURL(FString::Printf(TEXT("%s/oauth/huawei"), *URMProtocolFunctionLibrary::GetBaseUrl()));
			_payloadJson->SetStringField("id_token", *ID_Token);
			break;

		case ELoginWithParseType::Apple:
			HttpRequest->SetURL(FString::Printf(TEXT("%s/oauth/apple"), *URMProtocolFunctionLibrary::GetBaseUrl()));
			_payloadJson->SetStringField("id", *UserID);
			if (!ID_Token.IsEmpty())
				_payloadJson->SetStringField("token", *ID_Token);
			if (!SessionToken.IsEmpty())
				_payloadJson->SetStringField("sessionToken", *SessionToken);
			break;
	}

	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetVerb(TEXT("POST"));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginWithParseAPI::OnResponse);
		UE_LOG(LogRMProtocol, Warning, TEXT("%s - %s"), *HttpRequest->GetURL(), *_payload);
		UE_LOG(LogRMProtocol, Log, TEXT("LoginWithParse : %s"), *ID_Token);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("ULoginWithParseAPI - cannot process request"));
		Finished.Broadcast("", "", false, 0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void ULoginWithParseAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast("", "", false, 0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast("", "", false, Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
		
		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				
				URMProtocolFunctionLibrary::SetAccessToken(FString::Printf(TEXT("Bearer %s"), *data->GetStringField("accessToken")));
				
				Finished.Broadcast(data->GetStringField("uid"), data->GetStringField("sessionToken"), data->GetBoolField("newbie"), resultCode, true, TEXT(""));
			}
			else
			{	
				// Finished.Broadcast(FString::FromInt(resultCode), false, resultCode, false, res->GetStringField("resultMessage"));
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				Finished.Broadcast(data->GetStringField("uid"), "", false, resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UAccountConnectionWithParse* UAccountConnectionWithParse::AccountConnectionWithParse(ELoginWithParseType parseType, FString ID_Token, FString UserID)
{
	UAccountConnectionWithParse* BPNode = NewObject<UAccountConnectionWithParse>();
	BPNode->LoginWithParseType = parseType;
	BPNode->ID_Token = ID_Token;
	BPNode->UserID = UserID;
	return BPNode;
}

void UAccountConnectionWithParse::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

	switch (LoginWithParseType)
	{
		case ELoginWithParseType::Huawei:
			HttpRequest->SetURL(FString::Printf(TEXT("%s/oauth/sns/huawei"), *URMProtocolFunctionLibrary::GetBaseUrl()));
			_payloadJson->SetStringField("id_token", *ID_Token);
			break;

		case ELoginWithParseType::Apple:
			HttpRequest->SetURL(FString::Printf(TEXT("%s/oauth/sns/apple"), *URMProtocolFunctionLibrary::GetBaseUrl()));
			_payloadJson->SetStringField("id", *UserID);
			_payloadJson->SetStringField("token", *ID_Token);
			break;
	}

	HttpRequest->SetHeader(TEXT("content-type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAccountConnectionWithParse::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("LoginWithParse : %s"), *ID_Token);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UAccountConnectionWithParse - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}

}

void UAccountConnectionWithParse::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}
#pragma endregion


//--------------------------------------------------------------------------------------

UCreateAccountAPI* UCreateAccountAPI::CreateAccount(const FString& Club, const FString& Nickname, const FName& Flag, const FAvatarInfo2& AvatarInfo)
{
	UCreateAccountAPI* BPNode = NewObject<UCreateAccountAPI>();
	BPNode->club = Club;
	BPNode->Nickname = Nickname;
	BPNode->AvatarInfo = AvatarInfo;
	BPNode->Flag = Flag;
	return BPNode;
}

void UCreateAccountAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/account"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("thumbnail", club);
	_payloadJson->SetStringField("nickName", Nickname);
	_payloadJson->SetStringField("flagIcon", FString("0"));
	_payloadJson->SetStringField("base", AvatarInfo.BaseKey.ToString());
	_payloadJson->SetStringField("head", AvatarInfo.HeadKey.ToString());
	_payloadJson->SetStringField("hair", AvatarInfo.HairKey.ToString());
	_payloadJson->SetStringField("top", AvatarInfo.TopKey.ToString());
	_payloadJson->SetStringField("bottom", AvatarInfo.BottomKey.ToString());
	_payloadJson->SetStringField("leg", AvatarInfo.ShoesKey.ToString());
	_payloadJson->SetStringField("bodyType", AvatarInfo.BodyKey.ToString());

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCreateAccountAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("CreateAccount : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UCreateAccountAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UCreateAccountAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				URMProtocolFunctionLibrary::SetUserSeq(data->GetIntegerField("userSeq"));

				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

//--------------------------------------------------------------------------------------

FUserInfo MakeUserInfo(FHttpResponsePtr Response)
{
	FUserInfo userInfo;

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
			userInfo.UserID = data->GetStringField("uid");
			userInfo.Nickname = data->GetStringField("nickName");
			userInfo.Coin = data->GetIntegerField("coin");
			userInfo.VoiceCoin = data->GetIntegerField("voiceCoin");
			userInfo.Flag = FName(*data->GetStringField("flagIcon"));

			const TArray<TSharedPtr<FJsonValue>> costumes = data->GetArrayField("purchasedCostumes");
			for(TSharedPtr<FJsonValue> costume : costumes)
			{
				userInfo.PurchasedCostumes.Add(FName(*costume->AsString()));
			}
				
			const TArray<TSharedPtr<FJsonValue>> packs = data->GetArrayField("purchasedPacks");
			for (TSharedPtr<FJsonValue> pack : packs)
			{
				userInfo.PurchasedPacks.Add(FName(*pack->AsString()));
			}

			TSharedPtr<FJsonObject> avatar = data->GetObjectField(TEXT("avatarInfo"));
			userInfo.AvatarInfo.BaseKey = FName(*avatar->GetStringField("base"));
			userInfo.AvatarInfo.HeadKey = FName(*avatar->GetStringField("head"));
			userInfo.AvatarInfo.HairKey = FName(*avatar->GetStringField("hair"));
			userInfo.AvatarInfo.TopKey = FName(*avatar->GetStringField("top"));
			userInfo.AvatarInfo.BottomKey = FName(*avatar->GetStringField("bottom"));
			userInfo.AvatarInfo.ShoesKey = FName(*avatar->GetStringField("leg"));
			userInfo.AvatarInfo.BodyKey = FName(*avatar->GetStringField("bodyType"));

			TSharedPtr<FJsonObject> attendance = data->GetObjectField(TEXT("attendance"));
			userInfo.AttendanceSequence = attendance->GetIntegerField("no");
			userInfo.AttendanceFlag = attendance->GetIntegerField("flag") == 1;
		}
	}

	return userInfo;
}

UGetUserInfoAPI* UGetUserInfoAPI::GetUserInfo()
{
	UGetUserInfoAPI* BPNode = NewObject<UGetUserInfoAPI>();
	return BPNode;
}

void UGetUserInfoAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/me"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetUserInfoAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetUserInfo"));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetUserInfoAPI - cannot process request"));
		Finished.Broadcast(FUserInfo(), false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UGetUserInfoAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FUserInfo userInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(userInfo, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(userInfo, false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

				URMProtocolFunctionLibrary::SetUserSeq(data->GetIntegerField("userSeq"));
				URMProtocolFunctionLibrary::SetEncryptionUserSeq(data->GetStringField("encSeq"));
				userInfo.UserID = data->GetStringField("uid");
				userInfo.Nickname = data->GetStringField("nickName");
				userInfo.Coin = data->GetIntegerField("coin");
				userInfo.Crystal = data->GetIntegerField("crystal");
				userInfo.VoiceCoin = data->GetIntegerField("voiceCoin");
				userInfo.countrySP = FName(*data->GetStringField("countrySP"));
				GEngine->AddOnScreenDebugMessage(1, 10.0f, FColor::Magenta, FString(*data->GetStringField("countySP")));
				userInfo.Flag = FName(*data->GetStringField("flagIcon"));
				userInfo.Thumbnail = FName(*data->GetStringField("thumbnail"));
				userInfo.userState = data->GetIntegerField("userState");
				userInfo.provider = data->GetStringField("provider");
				userInfo.npcID = data->GetIntegerField("npcId");
				
				FDateTime::Parse(data->GetStringField("serverTime"), userInfo.ServerTime);
				FDateTime::Parse(data->GetStringField("accountEndTime"), userInfo.AccountEndTime);

				const TArray<TSharedPtr<FJsonValue>> costumes = data->GetArrayField("purchasedCostumes");
				for(TSharedPtr<FJsonValue> costume : costumes)
				{
					userInfo.PurchasedCostumes.Add(FName(*costume->AsString()));
				}
				
				const TArray<TSharedPtr<FJsonValue>> packs = data->GetArrayField("purchasedPacks");
				for (TSharedPtr<FJsonValue> pack : packs)
				{
					userInfo.PurchasedPacks.Add(FName(*pack->AsString()));
				}

				TSharedPtr<FJsonObject> avatar = data->GetObjectField(TEXT("avatarInfo"));
				userInfo.AvatarInfo.BaseKey = FName(*avatar->GetStringField("base"));
				userInfo.AvatarInfo.HeadKey = FName(*avatar->GetStringField("head"));
				userInfo.AvatarInfo.HairKey = FName(*avatar->GetStringField("hair"));
				userInfo.AvatarInfo.TopKey = FName(*avatar->GetStringField("top"));
				userInfo.AvatarInfo.BottomKey = FName(*avatar->GetStringField("bottom"));
				userInfo.AvatarInfo.ShoesKey = FName(*avatar->GetStringField("leg"));
				userInfo.AvatarInfo.BodyKey = FName(*avatar->GetStringField("bodyType"));

				TSharedPtr<FJsonObject> attendance = data->GetObjectField(TEXT("attendance"));
				userInfo.AttendanceSequence = attendance->GetIntegerField("no");
				userInfo.AttendanceFlag = attendance->GetIntegerField("flag") == 1;
				
				TArray<TSharedPtr<FJsonValue>> TicketListArr = data->GetArrayField("tickets");
				for (TSharedPtr<FJsonValue> TicketList : TicketListArr)
				{
					FTicketsInfo list;
					TSharedPtr<FJsonObject> attach = TicketList->AsObject();
					
					list.seq = attach->GetIntegerField("seq");
					list.CodeName = attach->GetStringField("codeName");
					list.iconImgUrl = attach->GetStringField("iconImgUrl");
					list.TotalCount = attach->GetIntegerField("totalCount");
					userInfo.Tickets.Add(list);
				}
				
				TSharedPtr<FJsonObject> AttendanceContinuation = data->GetObjectField(TEXT("attendanceContinuation"));
				userInfo.AttendContinuation.CountAttendance = AttendanceContinuation->GetIntegerField("countAttendance");
				userInfo.AttendContinuation.RewardNumber = AttendanceContinuation->GetIntegerField("rewardNumber");
				//check if attendancereward is not null

				if (AttendanceContinuation->HasField("reward"))
				{
					TArray<TSharedPtr<FJsonValue>> AttendanceReward = AttendanceContinuation->GetArrayField("reward");
				
					for (TSharedPtr<FJsonValue> reward : AttendanceReward)
					{
						FTicketReward list;
						list.TicketSeq = reward->AsObject()->GetIntegerField("ticketSeq");
						list.Total = reward->AsObject()->GetIntegerField("total");
						list.TicketCode = reward->AsObject()->GetStringField("ticketCode");
						list.IconImgUrl = reward->AsObject()->GetStringField("iconImgUrl");
						userInfo.AttendContinuation.TicketReward.Add(list);
					
					}
				}
				else
				{
					FTicketReward list;
					list.TicketSeq = 0;
					list.Total = 0;
					list.TicketCode = "";
					list.IconImgUrl = "";
					userInfo.AttendContinuation.TicketReward.Add(list);
				}

				TSharedPtr<FJsonObject> userActionPoint = data->GetObjectField(TEXT("userActionPoint"));
				userInfo.ActionPoint.point = userActionPoint->GetIntegerField("point");
				userInfo.ActionPoint.updatedAt = userActionPoint->GetStringField("updatedAt");

				
				FDateTime::Parse(data->GetStringField("accountEndTime"), URM_Singleton::GetSingleton(GetWorld())->TimeInfo.AccountEndTime);

				TSharedPtr<FJsonObject> customAvatarInfo = data->GetObjectField(TEXT("customAvatarInfo"));
				TSharedPtr<FJsonObject> baseInfo = customAvatarInfo->GetObjectField(TEXT("baseInfo"));
				FPlayerCharacter& PlayerCharacter = URM_Singleton::GetSingleton(GetWorld())->GetPlayerCharacter();
				if (baseInfo->GetIntegerField("gender") == 0){PlayerCharacter.Gender = EGenderRM::Male;}
				else if (baseInfo->GetIntegerField("gender") == 1){PlayerCharacter.Gender = EGenderRM::Female;}
				else if (baseInfo->GetIntegerField("gender") == 2){PlayerCharacter.Gender = EGenderRM::Male2;}
				else if (baseInfo->GetIntegerField("gender") == 3){PlayerCharacter.Gender = EGenderRM::Female2;}
				else if (baseInfo->GetIntegerField("gender") == 4){PlayerCharacter.Gender = EGenderRM::Male3;}
				else if (baseInfo->GetIntegerField("gender") == 5){PlayerCharacter.Gender = EGenderRM::Female3;}
				else if (baseInfo->GetIntegerField("gender") == 6){PlayerCharacter.Gender = EGenderRM::Male4;}
				else if (baseInfo->GetIntegerField("gender") == 7){PlayerCharacter.Gender = EGenderRM::Female4;}
				
				
				PlayerCharacter.TopSize = FCString::Atof(*baseInfo->GetStringField("topSize"));
				PlayerCharacter.BottomSize = FCString::Atof(*baseInfo->GetStringField("bottomSize"));
				PlayerCharacter.CheekSize = FCString::Atof(*baseInfo->GetStringField("cheekSize"));
				PlayerCharacter.Scale = FCString::Atof(*baseInfo->GetStringField("tallScale"));
				PlayerCharacter.HeadScale = FCString::Atof(*baseInfo->GetStringField("headScale"));
				PlayerCharacter.HandScale = FCString::Atof(*baseInfo->GetStringField("handScale"));
				PlayerCharacter.SkinColor = FColor::FromHex(*baseInfo->GetStringField("skinColor"));

				TSharedPtr<FJsonObject> costumeInfo = customAvatarInfo->GetObjectField(TEXT("costumeInfo"));
				PlayerCharacter.Costume[ECostumePart::Head] = FName(*costumeInfo->GetStringField("head"));
				PlayerCharacter.Costume[ECostumePart::Hair] = FName(*costumeInfo->GetStringField("hair"));
				PlayerCharacter.Costume[ECostumePart::Top] = FName(*costumeInfo->GetStringField("top"));
				PlayerCharacter.Costume[ECostumePart::Bottom] = FName(*costumeInfo->GetStringField("bottom"));
				PlayerCharacter.Costume[ECostumePart::Shoes] = FName(*costumeInfo->GetStringField("shoes"));
				// PlayerCharacter.Costume[ECostumePart::FaceSkin] = FName(*costumeInfo->GetStringField("faceSkin"));
				//PlayerCharacter.Costume[ECostumePart::EyeBrow] = FName(*costumeInfo->GetStringField("eyeBrow"));
				//PlayerCharacter.Costume[ECostumePart::Lip] = FName(*costumeInfo->GetStringField("lip"));

				TSharedPtr<FJsonObject> eyeInfo = customAvatarInfo->GetObjectField(TEXT("eyeInfo"));
				PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::EyeColor] = FCString::Atof(*eyeInfo->GetStringField("eyeColor"));
				PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::IrisBrightness] = FCString::Atof(*eyeInfo->GetStringField("irisBrightness"));
				PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::PupilScale] = FCString::Atof(*eyeInfo->GetStringField("pupilScale"));
				PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::ScleraBrightness] = FCString::Atof(*eyeInfo->GetStringField("scleraBrightness"));

				const TArray<TSharedPtr<FJsonValue>> tutorials = data->GetArrayField(TEXT("tutorials"));
				for (TSharedPtr<FJsonValue> list : tutorials)
				{
					TSharedPtr<FJsonObject> winner = list->AsObject();
					FString key = winner->GetStringField("type");
					int32 value = winner->GetIntegerField("val");

					userInfo.Tutorials.Add(key, value);
				}
				
				TSharedPtr<FJsonObject> morphInfo = customAvatarInfo->GetObjectField(TEXT("morphInfo"));
				int i = 0;
				for (auto MorphTarget : PlayerCharacter.MorphTargetValue)
				{
					FString str = EnumToString(TEXT("EMorphTarget"), i);
					str[0] = std::tolower(str[0]);
					str = str.Replace(TEXT(" "), TEXT(""), ESearchCase::Type::IgnoreCase);
					PlayerCharacter.MorphTargetValue[i++].Value = FCString::Atof(*morphInfo->GetStringField(str));
				}
				

				FPlayerCharacter& PlayerCharacter1 = URM_Singleton::GetSingleton(GetWorld())->GetPlayerCharacter(1);
				PlayerCharacter1 = PlayerCharacter;

				
				
				Finished.Broadcast(userInfo, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(userInfo, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

//--------------------------------------------------------------------------------------

UChangeNicknameAPI* UChangeNicknameAPI::ChangeNickname(const FString& Nickname)
{
	UChangeNicknameAPI* BPNode = NewObject<UChangeNicknameAPI>();
	BPNode->Nickname = Nickname;
	return BPNode;
}

void UChangeNicknameAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/nickname"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("nickName", Nickname);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UChangeNicknameAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ChangeNickname : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UChangeNicknameAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UChangeNicknameAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UUpdateUserStateAPI* UUpdateUserStateAPI::UpdateUserState(const int32& UserState)
{
	UUpdateUserStateAPI* BPNode = NewObject<UUpdateUserStateAPI>();
	BPNode->UserState = UserState;
	return BPNode;
}

void UUpdateUserStateAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/me"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	//_payloadJson->SetNumberField("userState", UserState);
	_payloadJson->SetStringField("userState", FString::FromInt(UserState));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUpdateUserStateAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UpdateeUserState : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("ChangeUserStateAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UUpdateUserStateAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UUpdateTeamSelectAPI* UUpdateTeamSelectAPI::UpdateTeamSelect(const FName& teamSelect)
{
	UUpdateTeamSelectAPI* BPNode = NewObject<UUpdateTeamSelectAPI>();
	BPNode->TeamSelect = teamSelect;
	return BPNode;
}

void UUpdateTeamSelectAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/me"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("thumbnail", TeamSelect.ToString());
	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString(TeamSelect.ToString()));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUpdateTeamSelectAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("TeamSelect : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UpdateTeamSelectAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UUpdateTeamSelectAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

//--------------------------------------------------------------------------------------



UUpdateAvatarInfoAPI* UUpdateAvatarInfoAPI::UpdateAvatarInfo(const FAvatarInfo2& AvatarInfo)
{
	UUpdateAvatarInfoAPI* BPNode = NewObject<UUpdateAvatarInfoAPI>();
	BPNode->AvatarInfo = AvatarInfo;
	return BPNode;
}

void UUpdateAvatarInfoAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/avatar"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("base", AvatarInfo.BaseKey.ToString());
	_payloadJson->SetStringField("head", AvatarInfo.HeadKey.ToString());
	_payloadJson->SetStringField("hair", AvatarInfo.HairKey.ToString());
	_payloadJson->SetStringField("top", AvatarInfo.TopKey.ToString());
	_payloadJson->SetStringField("bottom", AvatarInfo.BottomKey.ToString());
	_payloadJson->SetStringField("leg", AvatarInfo.ShoesKey.ToString());
	_payloadJson->SetStringField("bodyType", AvatarInfo.BodyKey.ToString());

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUpdateAvatarInfoAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UpdateAvatarInfo : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UUpdateAvatarInfoAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UUpdateAvatarInfoAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

//--------------------------------------------------------------------------------------

UBuyCostumeAPI* UBuyCostumeAPI::BuyCostume(const FName& CostumeKey)
{
	UBuyCostumeAPI* BPNode = NewObject<UBuyCostumeAPI>();
	BPNode->CostumeKey = CostumeKey.ToString();
	return BPNode;
}

void UBuyCostumeAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/buy/costumes/%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *CostumeKey));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBuyCostumeAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("BuyCostume : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UBuyCostumeAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UBuyCostumeAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}
	
	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

//--------------------------------------------------------------------------------------

UBuyContentPackAPI* UBuyContentPackAPI::BuyContentPack(const FName& CostumeKey)
{
	UBuyContentPackAPI* BPNode = NewObject<UBuyContentPackAPI>();
	BPNode->CostumeKey = CostumeKey.ToString();
	return BPNode;
}

void UBuyContentPackAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/buy/contents/%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *CostumeKey));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBuyContentPackAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("BuyContentPack : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UBuyContentPackAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UBuyContentPackAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}
	
	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}


//--------------------------------------------------------------------------------------

// UPurchaseGoogleAPI* UPurchaseGoogleAPI::PurchaseGoogle(const FString& receiptData)
// {
// 	UPurchaseGoogleAPI* BPNode = NewObject<UPurchaseGoogleAPI>();
// 	BPNode->receiptData = receiptData;
// 	BPNode->TargetSeq = 0;
// 	return BPNode;
// }
//
// UPurchaseGoogleAPI* UPurchaseGoogleAPI::GiftGoogle(const FString& receiptData, int32 TargetSeq)
// {
// 	UPurchaseGoogleAPI* BPNode = NewObject<UPurchaseGoogleAPI>();
// 	BPNode->receiptData = receiptData;
// 	BPNode->TargetSeq = TargetSeq;
// 	return BPNode;
// }
//
// void UPurchaseGoogleAPI::Activate()
// {
// 	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
// 	HttpRequest->SetURL(FString::Printf(TEXT("%s/purchase/google"), *URMProtocolFunctionLibrary::GetBaseUrl()));
// 	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
// 	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
// 	HttpRequest->SetVerb(TEXT("POST"));
//
// 	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
// 	_payloadJson->SetStringField("receiptData", *receiptData);
// 	
// 	if (TargetSeq != 0)
// 		_payloadJson->SetStringField("targetUserSeq", FString::FromInt(TargetSeq));
//
// 	FString _payload;
// 	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
// 	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
// 	HttpRequest->SetContentAsString(_payload);
//
// 	if (HttpRequest->ProcessRequest())
// 	{
// 		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPurchaseGoogleAPI::OnResponse);
// 		UE_LOG(LogRMProtocol, Log, TEXT("UPurchaseGoogleAPI : %s : %s"), *HttpRequest->GetURL(), *_payload);
// 	}
// 	else
// 	{
// 		UE_LOG(LogRMProtocol, Warning, TEXT("UPurchaseGoogleAPI - cannot process request"));
// 		Finished.Broadcast(0, false, TEXT("cannot process request"));
// 		SetReadyToDestroy();
// 	}
// }
//
// void UPurchaseGoogleAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
// {
// 	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
// 	if (Response == nullptr)
// 	{
// 		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
// 		SetReadyToDestroy();
// 		return;
// 	}
//
// 	if (!WasSuccessful || Response->GetResponseCode() != 200)
// 	{
// 		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
// 		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
// 		Finished.Broadcast(Response->GetResponseCode(), false, msg);
// 		SetReadyToDestroy();
// 		return;
// 	}
//
// 	//if (Response->GetResponseCode() == 200)
// 	{
// 		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
//
// 		TSharedPtr<FJsonObject> res;
// 		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
// 		if (FJsonSerializer::Deserialize(reader, res))
// 		{
// 			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
// 			{
// 				SetReadyToDestroy();
// 				return;
// 			}
// 			int resultCode = res->GetIntegerField("resultCode");
// 			if (resultCode == 200)
// 			{
// 				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
// 				if (data->HasField("accountEndTime"))
// 					FDateTime::Parse(data->GetStringField("accountEndTime"), URM_Singleton::GetSingleton(GetWorld())->TimeInfo.AccountEndTime);
// 				
// 				Finished.Broadcast(resultCode, true, TEXT(""));
// 			}
// 			else
// 			{
// 				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
// 			}
// 		}
// 	}
// 	SetReadyToDestroy();
// }

//--------------------------------------------------------------------------------------

UPurchaseAppleAPI* UPurchaseAppleAPI::PurchaseApple(const FString& ReceiptData)
{
	UPurchaseAppleAPI* BPNode = NewObject<UPurchaseAppleAPI>();
	BPNode->ReceiptData = ReceiptData;
	BPNode->TargetSeq = 0;
	return BPNode;
}

UPurchaseAppleAPI* UPurchaseAppleAPI::GiftApple(const FString& ReceiptData, int32 TargetSeq)
{
	UPurchaseAppleAPI* BPNode = NewObject<UPurchaseAppleAPI>();
	BPNode->ReceiptData = ReceiptData;
	BPNode->TargetSeq = TargetSeq;
	return BPNode;
}

void UPurchaseAppleAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/purchase/apple"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("receiptData", *ReceiptData);
	if (TargetSeq != 0)
		_payloadJson->SetStringField("targetUserSeq", FString::FromInt(TargetSeq));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPurchaseAppleAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UPurchaseAPI : %s : %s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPurchaseAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UPurchaseAppleAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				if (data->HasField("accountEndTime"))
					FDateTime::Parse(data->GetStringField("accountEndTime"), URM_Singleton::GetSingleton(GetWorld())->TimeInfo.AccountEndTime);
				
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}

//--------------------------------------------------------------------------------------

UPurchaseEditorAPI* UPurchaseEditorAPI::PurchaseEditor(const FString& ProductId)
{
	UPurchaseEditorAPI* BPNode = NewObject<UPurchaseEditorAPI>();
	BPNode->ProductId = ProductId;
	BPNode->TargetSeq = 0;
	return BPNode;
}

UPurchaseEditorAPI* UPurchaseEditorAPI::GiftEditor(const FString& ProductId, int32 TargetSeq)
{
	UPurchaseEditorAPI* BPNode = NewObject<UPurchaseEditorAPI>();
	BPNode->ProductId = ProductId;
	BPNode->TargetSeq = TargetSeq;
	return BPNode;
}

void UPurchaseEditorAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/purchase/editor"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("productId", *ProductId);
	if (TargetSeq != 0)
		_payloadJson->SetStringField("targetUserSeq", FString::FromInt(TargetSeq));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPurchaseEditorAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UPurchaseAPI : %s : %s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPurchaseAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UPurchaseEditorAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	int32 ErrorCode;
	FString ErrorMessage;

	if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *ErrorMessage);
		Finished.Broadcast(Response->GetResponseCode(), false, ErrorMessage);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	//if (Response->GetResponseCode() == 200)
	{
		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				/*TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				if (data->HasField("accountEndTime"))
					FDateTime::Parse(data->GetStringField("accountEndTime"), URM_Singleton::GetSingleton(GetWorld())->TimeInfo.AccountEndTime);*/
				
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}

//--------------------------------------------------------------------------------------

UAddCoinAPI* UAddCoinAPI::AddCoin(int32 Coin)
{
	UAddCoinAPI* BPNode = NewObject<UAddCoinAPI>();
	BPNode->Coin = Coin;
	return BPNode;
}

void UAddCoinAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/superAddUserCoin"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("coin", FString::FromInt(Coin));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAddCoinAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("AddCoin : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UAddCoinAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UAddCoinAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}
	
	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}


//--------------------------------------------------------------------------------------

UDeleteAccountAPI* UDeleteAccountAPI::DeleteAccount()
{
	UDeleteAccountAPI* BPNode = NewObject<UDeleteAccountAPI>();
	return BPNode;
}

void UDeleteAccountAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/secede"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDeleteAccountAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("DeleteAccount : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UDeleteAccountAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UDeleteAccountAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

USetCountryAPI* USetCountryAPI::SetCountry(const FName& Country)
{
	USetCountryAPI* BPNode = NewObject<USetCountryAPI>();
	BPNode->Country = Country;
	return BPNode;
}

void USetCountryAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/me"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("countrySP", Country.ToString());

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &USetCountryAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("SetFlag : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("USetFlagAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void USetCountryAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}


//--------------------------------------------------------------------------------------

USetFlagAPI* USetFlagAPI::SetFlag(const FName& Flag)
{
	USetFlagAPI* BPNode = NewObject<USetFlagAPI>();
	BPNode->Flag = Flag;
	return BPNode;
}

void USetFlagAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/flag"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("flagIcon", Flag.ToString());

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &USetFlagAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("SetFlag : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("USetFlagAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void USetFlagAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}


//--------------------------------------------------------------------------------------
/*
UCreatePartyRoomAPI* UCreatePartyRoomAPI::CreatePartyRoom(int32 ItemID)
{
	UCreatePartyRoomAPI* BPNode = NewObject<UCreatePartyRoomAPI>();
	BPNode->ItemID = ItemID;
	return BPNode;
}

void UCreatePartyRoomAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/room/party/%d"), *URMProtocolFunctionLibrary::GetBaseUrl(), ItemID));
	HttpRequest->SetHeader(TEXT("content-type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCreatePartyRoomAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("CreatePartyRoom : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UCreatePartyRoomAPI - cannot process request"));
	}
}

void UCreatePartyRoomAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 4001)
			{
				URMProtocolFunctionLibrary::OnMultipleAccessErrorDelegate.Broadcast();
			}
			else if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
}
*/





//--------------------------------------------------------------------------------------
/*
UDownloadConfigFile* UDownloadConfigFile::DownloadConfigFile()
{
	UDownloadConfigFile* BPNode = NewObject<UDownloadConfigFile>();
	return BPNode;
}

void UDownloadConfigFile::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
#if UE_BUILD_SHIPPING
	HttpRequest->SetURL(FString::Printf(TEXT("https://d39yhsvufud8qp.cloudfront.net/rm_config")));
#else
	HttpRequest->SetURL(FString::Printf(TEXT("http://106.255.240.98/data/Patch/JinKyu/test_config")));
#endif
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDownloadConfigFile::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("DownloadConfigFile : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UDownloadConfigFile - cannot process request"));
		Finished.Broadcast(false, TEXT("UDownloadConfigFile - cannot process request"));
	}
}

void UDownloadConfigFile::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FConfigInfo configInfo;

	// 네트워크가 끊어졌을때
	if (Response == nullptr)
	{
		UE_LOG(LogRMProtocol, Log, TEXT("if (Response == nullptr)"));
		Finished.Broadcast(-1, TEXT("The Internet connection appears to be offline."));
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), msg);
		return;
	}


	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
		
	FString storeLabel;
#if PLATFORM_IOS
	storeLabel = TEXT("Apple");
	configInfo.AppStore = EAppStore::E_Apple;
#else
	if (UKismetSystemLibrary::GetGameBundleId().Contains(TEXT("huawei")))
	{
		storeLabel = TEXT("Huawei");
		configInfo.AppStore = EAppStore::E_Huawei;
	}	
	else
	{
		storeLabel = TEXT("Google");
		configInfo.AppStore = EAppStore::E_Google;
	}
#endif

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		configInfo.ContentURL = res->GetStringField(TEXT("ContentURL"));

		const TArray<TSharedPtr<FJsonValue>> stores = res->GetArrayField("StoreSettings");
		for (TSharedPtr<FJsonValue> storeJson : stores)
		{
			TSharedPtr<FJsonObject> store = storeJson->AsObject();
			if (storeLabel.Equals(store->GetStringField(TEXT("Label"))))
			{
				configInfo.Version = store->GetStringField(TEXT("Version"));
				configInfo.Revision = store->GetIntegerField(TEXT("Revision"));
				configInfo.UpdateURL = store->GetStringField(TEXT("UpdateURL"));
				configInfo.ManifestURL = store->GetStringField(TEXT("ManifestURL"));
				configInfo.CloudURL = store->GetStringField(TEXT("CloudURL"));
				break;
			}
		}
	}

	URMBlueprintFunctionLibrary::SetConfig(configInfo);

	Finished.Broadcast(Response->GetResponseCode(), TEXT(""));
}*/


//--------------------------------------------------------------------------------------

UGetManifestList* UGetManifestList::GetManifestList()
{
	UGetManifestList* BPNode = NewObject<UGetManifestList>();
	return BPNode;
}

void UGetManifestList::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(*URMBlueprintFunctionLibrary::GetConfig().ManifestURL);
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetManifestList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetManifestList : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("GetManifestList - cannot process request"));

		TArray<FString> result;
		Finished.Broadcast(false, TEXT("cannot process request"), result, result);
		SetReadyToDestroy();
	}
}

void UGetManifestList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FString> result;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, TEXT("The Internet connection appears to be offline."), result, result);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, msg, result, result);
		SetReadyToDestroy();
		return;
	}


	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	FConfigInfo& ConfigInfo = URMBlueprintFunctionLibrary::GetConfigInfo();
	FString CloudURL = ConfigInfo.CloudURL;
	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		FString Version = res->GetStringField(TEXT("ClientVersion"));

		const TArray<TSharedPtr<FJsonValue>> files = res->GetArrayField("files");
		for (TSharedPtr<FJsonValue> fileJson : files)
		{
			TSharedPtr<FJsonObject> file = fileJson->AsObject();

			FString filename = file->GetStringField(TEXT("filename"));
			int index = filename.Find(Version);
			if (index != -1)
			{
				result.Add(CloudURL + filename);

				// Install folder
				filename = filename.Mid(filename.Find("pakchunk"));
				filename.RemoveFromEnd(Version + TEXT(".manifest"));

				ConfigInfo.InstallDirs.Add(TEXT("DLC\\") + filename);
			}
		}
	}

	Finished.Broadcast(true, CloudURL, result, ConfigInfo.InstallDirs);
	SetReadyToDestroy();
}

//--------------------------------------------------------------------------------------

#pragma region PurchaseHmsAPI
UPurchaseHmsAPI* UPurchaseHmsAPI::PurchaseHms(const FString& PurchaseToken, const FString& ProductId)
{
	UPurchaseHmsAPI* BPNode = NewObject<UPurchaseHmsAPI>();
	BPNode->PurchaseToken = PurchaseToken;
	BPNode->ProductId = ProductId;
	BPNode->TargetSeq = 0;
	return BPNode;
}

UPurchaseHmsAPI* UPurchaseHmsAPI::GiftHms(const FString& PurchaseToken, const FString& ProductId, int32 TargetSeq)
{
	UPurchaseHmsAPI* BPNode = NewObject<UPurchaseHmsAPI>();
	BPNode->PurchaseToken = PurchaseToken;
	BPNode->ProductId = ProductId;
	BPNode->TargetSeq = TargetSeq;
	return BPNode;
}

void UPurchaseHmsAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/purchase/huawei"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("purchaseToken", *PurchaseToken);
	_payloadJson->SetStringField("productId", *ProductId);
	if (TargetSeq != 0)
		_payloadJson->SetStringField("targetUserSeq", FString::FromInt(TargetSeq));
	
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPurchaseHmsAPI::OnResponse);
		UE_LOG(LogTemp, Log, TEXT("UPurchaseHmsAPI : %s : %s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPurchaseHmsAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UPurchaseHmsAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogTemp, Log, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
			if (data->HasField("accountEndTime"))
				FDateTime::Parse(data->GetStringField("accountEndTime"), URM_Singleton::GetSingleton(GetWorld())->TimeInfo.AccountEndTime);
			
			Finished.Broadcast(resultCode, true, TEXT(""));
		}
		else
		{
			Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
		}
	}
	SetReadyToDestroy();
}
#pragma endregion


//--------------------------------------------------------------------------------------

UTestHttp* UTestHttp::TestHttp()
{
	UTestHttp* BPNode = NewObject<UTestHttp>();
	return BPNode;
}

void UTestHttp::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("https://www.google.com")));
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTestHttp::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UTestHttp : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UTestHttp - cannot process request"));
		Finished.Broadcast(false);
		SetReadyToDestroy();
	}
}

void UTestHttp::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false);
		SetReadyToDestroy();
		return;
	}

	Finished.Broadcast(true);
	SetReadyToDestroy();
}

#pragma region AttendanceRewardAPI

UAttendanceRewardAPI* UAttendanceRewardAPI::AttendanceReward(const EQuestType& QuestType, const int32& QuestSequence)
{
	UAttendanceRewardAPI* BPNode = NewObject<UAttendanceRewardAPI>();
	BPNode->QuestType = QuestType;
	BPNode->QuestSequence = QuestSequence;
	return BPNode;
}

void UAttendanceRewardAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/quest/reward"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("rewardType", (int32)QuestType);
	_payloadJson->SetNumberField("rewardSeq", QuestSequence);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAttendanceRewardAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("AttendanceReward : %s\n%s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UAttendanceRewardAPI - cannot process request"));
		Finished.Broadcast(false, FUserInfo(), 0, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UAttendanceRewardAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FUserInfo UserInfo = MakeUserInfo(Response);

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, UserInfo, 0, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}
	
	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		// Finished.Broadcast(Response->GetResponseCode(), false, msg);
		Finished.Broadcast(false, UserInfo, Response->GetResponseCode(), msg);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
	
	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			Finished.Broadcast(true, UserInfo, resultCode, TEXT(""));
		}
		else
		{
			Finished.Broadcast(false, UserInfo, resultCode, res->GetStringField("resultMessage"));
		}
	}
	SetReadyToDestroy();
}

#pragma endregion



#pragma region VoiceCoin

//--------------------------------------------------------------------------------------

UBuyVoiceCoinAPI* UBuyVoiceCoinAPI::BuyVoiceCoin(const FName& ItemID)
{
	UBuyVoiceCoinAPI* BPNode = NewObject<UBuyVoiceCoinAPI>();
	BPNode->ItemID = ItemID.ToString();
	return BPNode;
}

void UBuyVoiceCoinAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/buy/voice/%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *ItemID));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBuyVoiceCoinAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("BuyVoiceCoin : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UBuyVoiceCoinAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UBuyVoiceCoinAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}


//--------------------------------------------------------------------------------------

UUseVoiceCoinAPI* UUseVoiceCoinAPI::UseVoiceCoin()
{
	UUseVoiceCoinAPI* BPNode = NewObject<UUseVoiceCoinAPI>();
	return BPNode;
}

void UUseVoiceCoinAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/use/voice"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUseVoiceCoinAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UseVoiceCoin : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UUseVoiceCoinAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UUseVoiceCoinAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

#pragma endregion


//--------------------------------------------------------------------------------------

UMultipleAccess* UMultipleAccess::CheckMultipleAccess(const FString& LevelName, const FString& RoomName)
{
	UMultipleAccess* BPNode = NewObject<UMultipleAccess>();
	BPNode->LevelName = LevelName;
	BPNode->RoomName = RoomName;
	return BPNode;
}

void UMultipleAccess::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/validation?room=%s&channel=%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *LevelName, *FGenericPlatformHttp::UrlEncode(RoomName)));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMultipleAccess::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UMultipleAccess : %s"), *HttpRequest->GetURL());

		
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UMultipleAccess - cannot process request"));
		Finished.Broadcast(false);
		SetReadyToDestroy();
	}
}

void UMultipleAccess::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false);
		SetReadyToDestroy();
		return;
	}
	
	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			/*int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 4001)
			{
				URMProtocolFunctionLibrary::OnMultipleAccessErrorDelegate.Broadcast();
			}
			else*/
			{
				Finished.Broadcast(true);
			}
		}
	}	
	SetReadyToDestroy();
}



//--------------------------------------------------------------------------------------

UGetPartyRoomList* UGetPartyRoomList::GetPartyRoomList(EPartySorting Sorting, int32 Page, int32 Count, FString KeyWord)
{
	UGetPartyRoomList* BPNode = NewObject<UGetPartyRoomList>();
	BPNode->Sorting = Sorting;
	BPNode->Page = Page;
	BPNode->Count = Count;
	BPNode->KeyWord = KeyWord;
	return BPNode;
}

void UGetPartyRoomList::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/room/list"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("page", FString::FromInt(Page));
	_payloadJson->SetStringField("showCnt", FString::FromInt(Count));
	_payloadJson->SetStringField("sortType", EPartySorting::Normal == Sorting ? TEXT("normal") : TEXT("like"));
	_payloadJson->SetStringField("keyword", KeyWord);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetPartyRoomList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetPartyRoomList - %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetPartyRoomList - cannot process request"));
		TArray<FPartyRoom> Rooms;
		Finished.Broadcast(false, 0, TEXT("cannot process request"), Rooms);
		SetReadyToDestroy();
	}
}

void UGetPartyRoomList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FPartyRoom> Rooms;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), Rooms);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, Rooms);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

				resultCode = data->GetIntegerField("totalPage");

				const TArray<TSharedPtr<FJsonValue>> list = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> j : list)
				{
					TSharedPtr<FJsonObject> room = j->AsObject();
					FPartyRoom info;
					info.RoomID = room->GetIntegerField("seq");
					info.RowName = FName(*room->GetStringField("itemID"));
					info.Title = room->GetStringField(TEXT("title"));
					info.UserName = room->GetStringField(TEXT("userName"));
					info.MasterUserSeq = room->GetIntegerField("userSeq");
					info.MaxUser = room->GetIntegerField("maxUser");
					info.LikeCount = room->GetIntegerField("likeCount");
					info.IsFixedRoom = room->GetIntegerField("type") == 0;
					info.IsShare = room->GetIntegerField("showYN") == 0;
					if (!info.IsShare)
						info.PW = room->GetStringField(TEXT("pwd"));
					info.CurrentUser = 0;

					FDateTime::Parse(room->GetStringField("endTime"), info.EndTime);

					Rooms.Add(info);
				}

				Finished.Broadcast(true, resultCode, TEXT(""), Rooms);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), Rooms);
			}
		}
	}
	SetReadyToDestroy();
}



//--------------------------------------------------------------------------------------

UCreatePartyRoom* UCreatePartyRoom::CreatePartyRoom(FString RoomName, FName RowName, bool IsShare, FString PW)
{
	UCreatePartyRoom* BPNode = NewObject<UCreatePartyRoom>();
	BPNode->RoomName = RoomName;
	BPNode->RowName = RowName;
	BPNode->IsShare = IsShare;
	BPNode->PW = PW;
	return BPNode;
}

void UCreatePartyRoom::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/room/buy"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("itemID", RowName.ToString());
	_payloadJson->SetStringField("title", RoomName);
	_payloadJson->SetStringField("pwd", PW);
	_payloadJson->SetStringField("showYN", IsShare ? TEXT("0") : TEXT("1"));
	

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCreatePartyRoom::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UCreatePartyRoom"));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UCreatePartyRoom - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"), FPartyRoom());
		SetReadyToDestroy();
	}
}

void UCreatePartyRoom::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FPartyRoom RoomInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), RoomInfo);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, RoomInfo);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

				TSharedPtr<FJsonObject> room = data->GetObjectField("room");
				RoomInfo.RoomID = room->GetIntegerField("seq");
				RoomInfo.RowName = FName(*room->GetStringField("itemID"));
				RoomInfo.Title = room->GetStringField(TEXT("title"));
				RoomInfo.UserName = room->GetStringField(TEXT("userName"));
				RoomInfo.MasterUserSeq = room->GetIntegerField("userSeq");
				RoomInfo.MaxUser = room->GetIntegerField("maxUser");
				RoomInfo.LikeCount = room->GetIntegerField("likeCount");
				RoomInfo.IsFixedRoom = room->GetIntegerField("type") == 0;
				RoomInfo.IsShare = room->GetIntegerField("showYN") == 0;
				if (!RoomInfo.IsShare)
					RoomInfo.PW = room->GetStringField(TEXT("pwd"));
				RoomInfo.CurrentUser = 0;

				FDateTime::Parse(room->GetStringField("endTime"), RoomInfo.EndTime);

				Finished.Broadcast(true, resultCode, TEXT(""), RoomInfo);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), RoomInfo);
			}
		}
	}
	SetReadyToDestroy();
}

//----------------------------------------------------------------------------------------------------------------------------

#pragma region EMAIL
UGetEmailList* UGetEmailList::GetEmailList(int lastEmailSeq)
{
	UGetEmailList* BPNode = NewObject<UGetEmailList>();
	BPNode->userEmailSeq = lastEmailSeq;
	return BPNode;
}

void UGetEmailList::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/email/list/%d"), *URMProtocolFunctionLibrary::GetBaseUrl(), userEmailSeq));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetEmailList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("%s"),  *(this->GetName()));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetEmailList - cannot process request"));
		TArray<FEmailData> emailDataArr;
		Finished.Broadcast(false, 0, TEXT("cannot process request"), emailDataArr);
		SetReadyToDestroy();
	}
}

void UGetEmailList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FEmailData> emailDataArr;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), emailDataArr);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, emailDataArr);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				const TArray<TSharedPtr<FJsonValue>> dataArr = res->GetArrayField(TEXT("data"));

				for (TSharedPtr<FJsonValue> data : dataArr)
				{
					FEmailData emailData;
					TSharedPtr<FJsonObject> email = data->AsObject();
					emailData.mailSeq = email->GetIntegerField("userEmailSeq");
					emailData.sendName = email->GetStringField("sendName");
					
					emailData.mailTitle = email->GetStringField("title");
					emailData.sendDate = email->GetStringField("regdate");
					emailData.expiryDate = email->GetStringField("expiryDate");
					emailData.haveAttach = email->GetIntegerField("haveAttach");

					// 0: 일반 메일
					// 1: 방 초대 메일
					// 100: GM 운영툴 보상 메일
					// 200: 랭킹 관련 보상 메일
					// 300: 경매 관련 메일
					int mailType = email->GetIntegerField("mailType");
					switch (mailType)
					{
						case 0:
						case 1:		emailData.mailType = (EEmailType)mailType;										break;				// 초대나, 일반 메일일 경우
						case 100:	emailData.mailType = EEmailType::RM_AccountConnectionReward;					break;				// GM 계정 연동 보상 메일일 경우
						case 101:	emailData.mailType = EEmailType::RM_Event_Reward;								break;				// 이벤트 보상
						case 102:	emailData.mailType = EEmailType::RM_Event_AttendReward;							break;				// 이벤트 참가 기본 보상
						case 103:	emailData.mailType = EEmailType::RM_Event_EndReward;							break;				// 이벤트 종료 기본 보상
						case 201:	emailData.mailType = EEmailType::RankingAttach_BallTrapping_Week;				break;				// BallTrapping Week Reward
						case 202:	emailData.mailType = EEmailType::RankingAttach_BallTrapping_Month;				break;				// BallTrapping Month Reward
						case 211:	emailData.mailType = EEmailType::RankingAttach_Dart_501_Week;					break;				// Dartt Week Reward
						case 212:	emailData.mailType = EEmailType::RankingAttach_Dart_501_Month;					break;				// Dart Month Reward
						case 300:	emailData.mailType = EEmailType::MarketAttachItem_Refund_By_Canceling;			break;				// 자신이 경매를 취소해서 받는 자신의 아이템
						case 301:	emailData.mailType = EEmailType::MarketAttachCoin_Refund_By_Canceling;			break;				// 판매자가 경매 취소로 인한 코인 환불
						case 302:	emailData.mailType = EEmailType::MarketAttachCoin_Failed_Bidding;				break;				// 상위 입찰자로 인한 환불
						case 303:	emailData.mailType = EEmailType::MarketAttachCoin_Failed_By_BuyingImmediately;	break;				// 즉시구매로 인한 환불
						case 304:	emailData.mailType = EEmailType::MarketAttachCoin_Success_Selling;				break;				// 판매성공에 따른 판매 대금
						case 305:	emailData.mailType = EEmailType::MarketAttachItem_Success_Buying;				break;				// 구매성공에 따른 구매 아이템
						case 400:	emailData.mailType = EEmailType::Receive_Gift;									break;				// 다른 유저에게 선물 받았을 경우
					}

					// 초대 메일일 경우
					if (emailData.mailType == EEmailType::Meta_Invite)
						emailData.roomTitle = email->GetStringField("roomTitle");

					// attach가 있는 경우
					if (emailData.haveAttach)
					{
						TArray<TSharedPtr<FJsonValue>> attachListArr = email->GetArrayField("attach");
						for (TSharedPtr<FJsonValue> attachList : attachListArr)
						{
							FAttachList list;
							TSharedPtr<FJsonObject> attach = attachList->AsObject();
							

							list.attachmentSeq = attach->GetIntegerField("userEmailAttachFileSeq");
							list.itemID = attach->GetIntegerField("itemID");
							list.isAlreadyGet = (bool)attach->GetIntegerField("getFlag");
							int itemType = attach->GetIntegerField("itemType");
							switch (itemType)
							{
								case 0: list.itemType = EItemType::Goods;		break;		// Coin
								case 1: list.itemType = EItemType::Costume;		break;		// Costume
								case 2: list.itemType = EItemType::VoiceCoin;	break;		// VoiceCoin
								//case 5: list.itemType = EItemType::Membership;	break;		// Membership
								default:list.itemType = EItemType::Goods;		break;		// Default?
							}

							UE_LOG(LogTemp, Warning, TEXT("itemId: %d, AlreadyGet: %d, itemType: %d"), list.itemID, list.isAlreadyGet, list.itemType);

							if (!list.isAlreadyGet)
								emailData.attachList.Add(list);
						}
						if (emailData.attachList.Num() == 0)
							continue;
					}

					emailDataArr.Add(emailData);
				}
				Finished.Broadcast(true, resultCode, TEXT(""), emailDataArr);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), emailDataArr);
			}
		}
	}
	SetReadyToDestroy();
}


UGetEmailAttachment* UGetEmailAttachment::GetAttachment(int userEmailSeq, int AttachmentSeq, EItemType itemType, int itemID)
{
	UGetEmailAttachment* BPNode = NewObject<UGetEmailAttachment>();
	BPNode->userEmailSeq = userEmailSeq;
	BPNode->attachmentSeq = AttachmentSeq;
	BPNode->itemType = itemType;
	BPNode->itemID = itemID;
	return BPNode;
}

void UGetEmailAttachment::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/email/attach"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("userEmailSeq", FString::FromInt(userEmailSeq));
	_payloadJson->SetStringField("userEmailAttachFileSeq", FString::FromInt(attachmentSeq));
	switch (itemType)
	{
		case EItemType::None		: _payloadJson->SetStringField("itemType", FString::FromInt(-1)); break;
		case EItemType::Goods		: _payloadJson->SetStringField("itemType", FString::FromInt(0)); break;
		case EItemType::Costume		: _payloadJson->SetStringField("itemType", FString::FromInt(1)); break;
		case EItemType::VoiceCoin	: _payloadJson->SetStringField("itemType", FString::FromInt(2)); break;
		//case EItemType::Membership	: _payloadJson->SetStringField("itemType", FString::FromInt(5)); break;
	}

	_payloadJson->SetStringField("itemID", FString::FromInt(itemID));

	FString _payload;
	TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetEmailAttachment::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetEmailAttachment : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Log, TEXT("UGetEmailAttachment - Can't Process Request"));
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
	}
}

void UGetEmailAttachment::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}
#pragma endregion

//--------------------------------------------------------------------------------------

UIsExistRoom* UIsExistRoom::IsExistRoom(FString RoomTitle, int32 RoomID)
{
	UIsExistRoom* BPNode = NewObject<UIsExistRoom>();
	BPNode->RoomID = RoomID;
	BPNode->RoomTitle = RoomTitle;
	return BPNode;
}

void UIsExistRoom::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/room/check"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	
	if (RoomTitle == TEXT(""))
		_payloadJson->SetStringField("seq", FString::FromInt(RoomID));
	else
		_payloadJson->SetStringField("title", RoomTitle);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UIsExistRoom::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UIsExistRoom : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UIsExistRoom - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), FPartyRoom());
		SetReadyToDestroy();
	}
}

void UIsExistRoom::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FPartyRoom RoomInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), RoomInfo);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, RoomInfo);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");

			TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
			
			RoomInfo.RoomID = data->GetIntegerField("seq");
			RoomInfo.RowName = FName(*data->GetStringField("itemID"));
			RoomInfo.Title = data->GetStringField(TEXT("title"));
			RoomInfo.UserName = data->GetStringField(TEXT("userName"));
			RoomInfo.MasterUserSeq = data->GetIntegerField("userSeq");
			RoomInfo.MaxUser = data->GetIntegerField("maxUser");
			RoomInfo.LikeCount = data->GetIntegerField("likeCount");
			RoomInfo.IsFixedRoom = data->GetIntegerField("type") == 0;
			RoomInfo.IsShare = data->GetIntegerField("showYN") == 0;
			if (!RoomInfo.IsShare)
				RoomInfo.PW = data->GetStringField(TEXT("pwd"));
			RoomInfo.CurrentUser = 0;

			FDateTime::Parse(data->GetStringField("endTime"), RoomInfo.EndTime);

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""), RoomInfo);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), RoomInfo);
			}
		}
	}
	SetReadyToDestroy();
}



//--------------------------------------------------------------------------------------

ULikePartyRoom* ULikePartyRoom::LikePartyRoom(int32 RoomID)
{
	ULikePartyRoom* BPNode = NewObject<ULikePartyRoom>();
	BPNode->RoomID = RoomID;
	return BPNode;
}

void ULikePartyRoom::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/room/like"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("seq", FString::FromInt(RoomID));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULikePartyRoom::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ULikePartyRoom : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("ULikePartyRoom - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void ULikePartyRoom::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}




//--------------------------------------------------------------------------------------

UGetRecommendedUsers* UGetRecommendedUsers::GetRecommendedUsers()
{
	UGetRecommendedUsers* BPNode = NewObject<UGetRecommendedUsers>();
	return BPNode;
}

void UGetRecommendedUsers::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/game/user-recommend"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetRecommendedUsers::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetRecommendedUsers"));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetRecommendedUsers - cannot process request"));
		TArray<FSearchedUser> Users;
		Finished.Broadcast(false, 0, TEXT("cannot process request"), Users);
		SetReadyToDestroy();
	}
}

void UGetRecommendedUsers::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FSearchedUser> Users;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), Users);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, Users);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				const TArray<TSharedPtr<FJsonValue>> list = res->GetArrayField(TEXT("data"));
				for (TSharedPtr<FJsonValue> j : list)
				{
					TSharedPtr<FJsonObject> obj = j->AsObject();
					FSearchedUser user;
					user.UserSeq = obj->GetIntegerField("userSeq");
					user.Nickname = obj->GetStringField("nickName");
					user.Flag = FName(*obj->GetStringField("flagIcon"));

					Users.Add(user);
				}

				Finished.Broadcast(true, resultCode, TEXT(""), Users);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), Users);
			}
		}
	}
	SetReadyToDestroy();
}




//--------------------------------------------------------------------------------------

UFindUser* UFindUser::FindUser(FString Nickname)
{
	UFindUser* BPNode = NewObject<UFindUser>();
	BPNode->Nickname = Nickname;
	return BPNode;
}

void UFindUser::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/game/user-search"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("nickName", Nickname);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UFindUser::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UFindUser"));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UFindUser - cannot process request"));
		TArray<FSearchedUser> Users;
		Finished.Broadcast(false, 0, TEXT("cannot process request"), Users);
		SetReadyToDestroy();
	}
}

void UFindUser::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FSearchedUser> Users;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), Users);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, Users);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				const TArray<TSharedPtr<FJsonValue>> list = res->GetArrayField(TEXT("data"));
				for (TSharedPtr<FJsonValue> j : list)
				{
					TSharedPtr<FJsonObject> obj = j->AsObject();
					FSearchedUser user;
					user.UserSeq = obj->GetIntegerField("userSeq");
					user.Nickname = obj->GetStringField("nickName");
					user.Flag = FName(*obj->GetStringField("flagIcon"));

					Users.Add(user);
				}

				Finished.Broadcast(true, resultCode, TEXT(""), Users);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), Users);
			}
		}
	}
	SetReadyToDestroy();
}



//--------------------------------------------------------------------------------------

UInviteUser* UInviteUser::InviteUser(int32 UserSeq, int32 RoomID)
{
	UInviteUser* BPNode = NewObject<UInviteUser>();
	BPNode->UserSeq = UserSeq;
	BPNode->RoomID = RoomID;
	return BPNode;
}

void UInviteUser::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/game/user-invite"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PUT"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("userSeq", FString::FromInt(UserSeq));
	_payloadJson->SetStringField("roomSeq", FString::FromInt(RoomID));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UInviteUser::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UInviteUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UInviteUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UInviteUser::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------

#pragma region ULoadMiniGameInfo
ULoadMiniGameInfo* ULoadMiniGameInfo::LoadMiniGameInfo(int32 page, int32 loadCountInOnePage, EGameRankType rankType, EGameType gameSeq)
{
	ULoadMiniGameInfo* BPNode = NewObject<ULoadMiniGameInfo>();
	BPNode->requestPage = page;
	BPNode->loadCount = loadCountInOnePage;
	BPNode->rankType = rankType;
	BPNode->gameType = gameSeq;

	return BPNode;
}

void ULoadMiniGameInfo::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Set Field
	int gameSeq = (int)gameType;
	FString requestRankType = TEXT("");
	if (rankType == EGameRankType::RANKTYPE_Month)
		requestRankType = TEXT("month");
	else if (rankType == EGameRankType::RANKTYPE_Week)
		requestRankType = TEXT("week");

	// Set Http Request
	HttpRequest->SetURL(FString::Printf(TEXT("%s/game/score/list"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	// Set Http Request Field Type
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("gameSeq", gameSeq);
	_payloadJson->SetNumberField("page", requestPage < 0 ? 1 : requestPage);
	_payloadJson->SetNumberField("showCnt", loadCount <= 0 ? 10 : loadCount);
	_payloadJson->SetStringField("type", requestRankType);

	// Serialize jsonData
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	
	// Process Request
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoadMiniGameInfo::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ULoadMiniGameInfo : %s"), *HttpRequest->GetURL());
		UE_LOG(LogTemp, Log, TEXT("Request Contents: %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("ULoadMiniGameInfo - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), FMiniGameInfo());
		SetReadyToDestroy();
	}
}

void ULoadMiniGameInfo::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FMiniGameInfo gameInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), gameInfo);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, gameInfo);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				FMiniGamePageInfo pageInfo;
				FMiniGameUserInfo myInfo;
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TSharedPtr<FJsonObject> pagination = data->GetObjectField(TEXT("pagination"));
				TSharedPtr<FJsonObject> myData = data->GetObjectField(TEXT("me"));

				// 1. Get pageInfo
				pageInfo.currentPage = pagination->GetIntegerField(TEXT("currentPage"));
				pageInfo.prevPage = pagination->GetIntegerField(TEXT("prevPage"));
				pageInfo.nextPage = pagination->GetIntegerField(TEXT("nextPage"));
				pageInfo.loadCount = pagination->GetIntegerField(TEXT("limit"));
				pageInfo.totalPlayer = pagination->GetIntegerField(TEXT("total"));
				pageInfo.firstPage = pagination->GetIntegerField(TEXT("firstPage"));
				pageInfo.lastPage = pagination->GetIntegerField(TEXT("lastPage"));

				// 2. Get myData
				myInfo.userSeq = URMProtocolFunctionLibrary::GetUserSeq();
				myInfo.rank = myData->GetIntegerField(TEXT("rank"));
				// myInfo.nickName = myData->GetStringField(TEXT("nickName"));
				myData->TryGetStringField(TEXT("nickName"), myInfo.nickName);
				myInfo.score = myData->GetStringField(TEXT("score"));
				myInfo.regDate = myData->GetStringField(TEXT("regdate"));

				// 3. Get Request UserInfo
				// first rank Index.
				int32 rank = 1 + ((pageInfo.currentPage - 1) * pageInfo.loadCount);
				const TArray<TSharedPtr<FJsonValue>> listArr = data->GetArrayField(TEXT("list"));
				for (TSharedPtr<FJsonValue>list : listArr)
				{
					FMiniGameUserInfo userInfo;
					TSharedPtr<FJsonObject> info_json = list->AsObject();
					userInfo.userSeq = info_json->GetIntegerField(TEXT("userSeq"));
					userInfo.score = info_json->GetStringField(TEXT("score"));
					// userInfo.nickName = info_json->GetStringField(TEXT("nickName"));
					info_json->TryGetStringField(TEXT("nickName"), userInfo.nickName);
					userInfo.regDate = info_json->GetStringField(TEXT("regdate"));
					userInfo.rank = rank++;

					// Added Item
					gameInfo.userInfoArr.Add(userInfo);
				}

				// 4. Set Info Data.
				gameInfo.pageInfo = pageInfo;
				gameInfo.myInfo = myInfo;

				Finished.Broadcast(resultCode, true, TEXT(""), gameInfo);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), gameInfo);
			}
		}
	}
	SetReadyToDestroy();
}
#pragma endregion

//-------------------------------------------------------------------------------------------------------------------

#pragma region USaveMiniGame
USaveMiniGameInfo* USaveMiniGameInfo::SaveMiniGameInfo(EGameType gameSeq, int32 score)
{
	USaveMiniGameInfo* BPNode = NewObject<USaveMiniGameInfo>();
	BPNode->gameSeq = (int32)gameSeq;
	BPNode->score = score;
	return BPNode;
}

void USaveMiniGameInfo::Activate()
{
	FString ScoreInfo;
	switch (gameSeq)
	{
		case (int)EGameType::BallTrapping: 
			ScoreInfo = FString::FromInt(score);
			break;
		case (int)EGameType::Dart_501: 
			ScoreInfo = FString::FromInt(score / 10) + TEXT("-") + FString::FromInt(score % 10);
			UE_LOG(LogTemp, Warning, TEXT("Dart_501 Save Score : %s"), *ScoreInfo);
			break;
		case (int)EGameType::FreeKick:
			ScoreInfo = FString::FromInt(score);
			break;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/game/score"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField(TEXT("gameSeq"), gameSeq);
	_payloadJson->SetStringField(TEXT("score"), ScoreInfo);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &USaveMiniGameInfo::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ULoadMiniGameInfo : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("USaveMiniGameInfo::Process Error Request."));
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
	}
}

void USaveMiniGameInfo::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField(TEXT("resultCode"));
			if (resultCode == 200)
				Finished.Broadcast(resultCode, true, TEXT(""));
			else
				Finished.Broadcast(resultCode, false, res->GetStringField(TEXT("resultMessage")));
		}
	}
	SetReadyToDestroy();
}
#pragma endregion


//--------------------------------------------------------------------------------------

UReportUser* UReportUser::ReportUser(FString Nickname, FString Desc)
{
	UReportUser* BPNode = NewObject<UReportUser>();
	BPNode->Nickname = Nickname;
	BPNode->Desc = Desc;
	return BPNode;
}

void UReportUser::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/report"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("nickName", Nickname);
	_payloadJson->SetStringField("contents", Desc);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UReportUser::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UReportUser::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}


#pragma region RWWARD_TABLE
/*
* 
* 
*/
UGetRewardTable* UGetRewardTable::GetRewardTable()
{
	UGetRewardTable* BPNode = NewObject<UGetRewardTable>();
	return BPNode;
}

void UGetRewardTable::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/game/reward-table"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetRewardTable::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetRewardTable : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("GetRewardTable - cannot process request"));
		TArray<FRewardTable> tableArr;
		Finished.Broadcast(0, false, TEXT("cannot process request"), tableArr);
		SetReadyToDestroy();
	}
}

void UGetRewardTable::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FRewardTable> tableArr;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), tableArr);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, tableArr);
		SetReadyToDestroy();
		return;
	}

	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			TArray<TSharedPtr<FJsonValue>> arr = res->GetArrayField("data");
			if (arr.Num() > 0)
			{
				for (TSharedPtr<FJsonValue> child : arr)
				{
					TSharedPtr<FJsonObject> obj = child->AsObject();
					FRewardTable table;
					table.gameSeq = (EGameType)obj->GetIntegerField("gameSeq");
					table.rankType = (EGameRankType)obj->GetIntegerField("rankType");
					table.rankGrade = obj->GetIntegerField("rankGrade");
					table.rewardID = obj->GetIntegerField("rewardID");
					table.rewardCount = obj->GetIntegerField("rewardCount");
					table.rewardReplacementPrice = obj->GetIntegerField("rewardReplacementPrice");
					table.activated = obj->GetBoolField("activated");
					int rewardType = obj->GetIntegerField("rewardType");
					switch (rewardType)
					{
						case 0: table.rewardType = EItemType::Goods;	break;
						case 1: table.rewardType = EItemType::Costume;	break;
					}

					tableArr.Add(table);
				}
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""), tableArr);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), tableArr);
			}
		}
	}
	SetReadyToDestroy();
}
#pragma endregion

#pragma region TEST_REWARD
UTestReward* UTestReward::SendTestReward(EGameType gameType, EGameRankType rankType)
{
	UTestReward* BPNode = NewObject<UTestReward>();
	BPNode->GameType = gameType;
	BPNode->RankType = rankType;
	return BPNode;
}

void UTestReward::Activate()
{
	int requestGameType = (int)GameType;
	int requestRankType = (int)RankType;

	FHttpRequestPtr request = FHttpModule::Get().CreateRequest();
	request->SetURL(FString::Printf(TEXT("%s/game/score-reward-test?game=%d&type=%d"), *URMProtocolFunctionLibrary::GetBaseUrl(), requestGameType, requestRankType));
	request->SetVerb(TEXT("GET"));
	request->SetHeader(TEXT("content-type"), TEXT("application/json"));
	request->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());

	if (request->ProcessRequest())
	{
		request->OnProcessRequestComplete().BindUObject(this, &UTestReward::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UTestReward : %s"), *request->GetURL());
	}
	else
	{
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UTestReward::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful)
	{
		UE_LOG(LogRMProtocol, Error, TEXT("UTestReward::OnResponse Error"));
		Finished.Broadcast(Response->GetResponseCode(), WasSuccessful, TEXT("ERROR!"));
		SetReadyToDestroy();
		return;
	}
	
	if (Response->GetResponseCode() == 200)
	{
		Finished.Broadcast(Response->GetResponseCode(), WasSuccessful, TEXT("SUCCEEDED"));
	}
	else
	{
		Finished.Broadcast(Response->GetResponseCode(), WasSuccessful, TEXT("FALSE"));
	}
	SetReadyToDestroy();
}

UUpdateWinner* UUpdateWinner::UpdateWinner(FString WinnerNum)
{
	UUpdateWinner* BPNode = NewObject<UUpdateWinner>();
	BPNode->Winner = WinnerNum;

	return BPNode;
}

void UUpdateWinner::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/win"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("opponentUserSeq", Winner);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUpdateWinner::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UUpdateWinner::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UGetCardRankArray* UGetCardRankArray::GetCardRankArray()
{
	UGetCardRankArray* BPNode = NewObject<UGetCardRankArray>();
	return BPNode;
}

void UGetCardRankArray::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/rank"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));
	TArray<FCardRankInfo> ResultItems;
	ResultItems.Empty();

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetCardRankArray::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetCardRank "));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyWatchItem - cannot process request"));
		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("cannot process request"), ResultItems);
		SetReadyToDestroy();
	}
}

void UGetCardRankArray::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TArray<FCardRankInfo> ResultItems;
		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ResultItems);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		TArray<FCardRankInfo> ResultItems;
		ResultItems.Empty();
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg,ResultItems);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		TArray<FCardRankInfo> ResultItems;
		ResultItems.Empty();

		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");

			if (resultCode == 200)
			{
				if (res->HasField(TEXT("data")))
				{
					TArray<TSharedPtr<FJsonValue>> ResultArray = res->GetArrayField("data");
					for (auto Elem : ResultArray)
					{
						TSharedPtr<FJsonObject> Info = Elem->AsObject();
						FCardRankInfo ArrayElem;
						ArrayElem.nickName = Info->GetStringField("nickName");
						ArrayElem.wincount = Info->GetIntegerField("winCnt");

						ResultItems.Add(ArrayElem);
					}

					Finished.Broadcast(resultCode, true, TEXT(""), ResultItems);
				}
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), ResultItems);
			}
		}
	}
	SetReadyToDestroy();
}

UInputEmail* UInputEmail::InputEmail(FString Email)
{
	UInputEmail* BPNode = NewObject<UInputEmail>();
	BPNode->Email = Email;

	return BPNode;
}

void UInputEmail::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/event/my-info"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("email", Email);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UInputEmail::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UInputEmail::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UGoogleLinkAPI* UGoogleLinkAPI::SetGoogleLink(FString Link)
{
	UGoogleLinkAPI* BPNode = NewObject<UGoogleLinkAPI>();
	BPNode->Link = Link;
//UpdateUserState
	return BPNode;
}

void UGoogleLinkAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/link/google"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetHeader(TEXT("Proxy-Authorization"), FString::Printf(TEXT("Bearer %s"),*Link));
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGoogleLinkAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GoogleLink : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGoogleLinkAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UGoogleLinkAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());
		
		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{	
				// Finished.Broadcast(FString::FromInt(resultCode), false, resultCode, false, res->GetStringField("resultMessage"));
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

USetCardTutorialAPI* USetCardTutorialAPI::USetCardTutorial(const int32 Tutorial)
{
	USetCardTutorialAPI* BPNode = NewObject<USetCardTutorialAPI>();
	BPNode->Tutorial = Tutorial;

	return BPNode;
}
void USetCardTutorialAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/cardTutorial"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("cardTutorial", Tutorial);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &USetCardTutorialAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void USetCardTutorialAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UDailyQuestAPI* UDailyQuestAPI::DailyQuest()
{
	UDailyQuestAPI* BPNode = NewObject<UDailyQuestAPI>();
	return BPNode;
}
void UDailyQuestAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/quest/my-today-reward"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));
	FDailyQuestInfo QuestInfo;

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDailyQuestAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetDailyQuestInfo "));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyWatchItem - cannot process request"));
		Finished.Broadcast(0,false, TEXT("cannot process request"),QuestInfo);
		SetReadyToDestroy();
	}
}

void UDailyQuestAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		FDailyQuestInfo ResultItems;
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."),ResultItems);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FDailyQuestInfo ResultItems;
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg,ResultItems);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		FDailyQuestInfo ResultItems;

		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");

			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				ResultItems.dart = data->GetIntegerField("dart");
				ResultItems.freekick = data->GetIntegerField("freekick");
				ResultItems.card = data->GetIntegerField("card");
				Finished.Broadcast(resultCode, true, TEXT(""), ResultItems);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), ResultItems);
			}
		}
	}
	SetReadyToDestroy();
}

UScreenDisplayAPI* UScreenDisplayAPI::ScreenDisplay()
{
	UScreenDisplayAPI* BPNode = NewObject<UScreenDisplayAPI>();
	return BPNode;
}
void UScreenDisplayAPI::Activate()
{
	TArray<FScreenDisplayInfo> ScreenInfos;
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/info/screen"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UScreenDisplayAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ScreenInfos : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("ScreenDisplay - cannot process request"));
		Finished.Broadcast(0, 0, TEXT("cannot process request"), ScreenInfos);
		SetReadyToDestroy();
	}
}

void UScreenDisplayAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FScreenDisplayInfo> ScreenInfos;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, 0, TEXT("The Internet connection appears to be offline."),ScreenInfos );
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false,msg, ScreenInfos);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMsg = res->GetStringField("resultMessage");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TArray<TSharedPtr<FJsonValue>> listArr = data->GetArrayField("screenlist");
				for (TSharedPtr<FJsonValue> item : listArr)
				{
					TSharedPtr<FJsonObject> obj = item->AsObject();
					if (obj)
					{
						FScreenDisplayInfo info;
						info.seq = (obj->HasField("seq")) ? obj->GetIntegerField("seq") : -1;
						info.title = (obj->HasField("title")) ? obj->GetStringField("title") : TEXT("");
						info.url = (obj->HasField("url")) ? obj->GetStringField("url") : TEXT("");
						info.area = (obj->HasField("area")) ? obj->GetStringField("area") : TEXT("");

						ScreenInfos.Add(info);
					}
				}
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, 1, TEXT(""), ScreenInfos);
			}
			else
			{
				Finished.Broadcast(resultCode, 1, res->GetStringField("resultMessage"), ScreenInfos);
			}
		}
	}
}

#pragma endregion




#pragma region Market

//--------------------------------------------------------------------------------------

URegisterMarket* URegisterMarket::RegisterMarket(FName ItemKey, FString Title, int32 StartPrice, int32 BuyPrice, int32 Period, EAuctionCategory Category)
{
	URegisterMarket* BPNode = NewObject<URegisterMarket>();
	BPNode->ItemKey = ItemKey;
	BPNode->Title = Title;
	BPNode->StartPrice = StartPrice;
	BPNode->BuyPrice = BuyPrice;
	BPNode->Period = Period;
	BPNode->Category = Category;
	return BPNode;
}

void URegisterMarket::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/product"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("itemType", "1");
	_payloadJson->SetStringField("itemID", ItemKey.ToString());
	switch (Category)
	{
	case EAuctionCategory::Shirts:
		_payloadJson->SetStringField("category", TEXT("11"));
		break;
	case EAuctionCategory::Pants:
		_payloadJson->SetStringField("category", TEXT("12"));
		break;
	case EAuctionCategory::Shoes:
		_payloadJson->SetStringField("category", TEXT("13"));
		break;
	case EAuctionCategory::ETC:
		_payloadJson->SetStringField("category", TEXT("14"));
		break;
	}
	_payloadJson->SetStringField("title", Title);
	_payloadJson->SetStringField("buyitNow", FString::FromInt(BuyPrice));
	_payloadJson->SetStringField("currentBid", FString::FromInt(StartPrice));
	_payloadJson->SetStringField("durationDate", FString::FromInt(Period));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URegisterMarket::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("RegisterMarket : %s, %s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("URegisterMarket - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void URegisterMarket::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

//--------------------------------------------------------------------------------------

FAuctionItem MakeAuctionItem(TSharedPtr<FJsonObject> data)
{
	FAuctionItem item;

	item.AuctionNumber = data->GetStringField("auctionNumber");
	item.ItemID = FName(*data->GetStringField("itemID"));
	item.SellUserSeq = data->GetIntegerField("sellUserSeq");
	item.BidUserSeq = data->GetIntegerField("bidUserSeq");

	item.Title = data->GetStringField("title");

	item.BuyPrice = data->GetIntegerField("buyitNow");
	item.CurrentBidPrice = data->GetIntegerField("currentBid");
	item.WatchCount = data->GetIntegerField("watchs");
	item.BidCount = data->GetIntegerField("bids");

	FDateTime::Parse(data->GetStringField("regdate"), item.RegDate);
	FDateTime::Parse(data->GetStringField("endDate"), item.EndDate);

	item.Status = data->GetIntegerField("status");

	return item;
}



//--------------------------------------------------------------------------------------

UUnRegisterMarket* UUnRegisterMarket::UnRegisterMarket(FString AuctionNumber)
{
	UUnRegisterMarket* BPNode = NewObject<UUnRegisterMarket>();
	BPNode->AuctionNumber = AuctionNumber;
	return BPNode;
}

void UUnRegisterMarket::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/product"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("auctionNumber", AuctionNumber);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUnRegisterMarket::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UnRegisterMarket : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UnRegisterMarket - cannot process request"));
		Finished.Broadcast(0, TEXT("cannot process request"), FAuctionItem(), 0);
		SetReadyToDestroy();
	}
}

void UUnRegisterMarket::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FAuctionItem auctionItem;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, TEXT("The Internet connection appears to be offline."), auctionItem, 0);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), msg, auctionItem, 0);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		int32 PenaltyCoin = 0;

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			
			
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				auctionItem = MakeAuctionItem(data->GetObjectField(TEXT("marketProduct")));
				PenaltyCoin = data->GetIntegerField("penaltyCoin");
			}
				
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, TEXT(""), auctionItem, PenaltyCoin);
			}
			else
			{
				Finished.Broadcast(resultCode, res->GetStringField("resultMessage"), auctionItem, PenaltyCoin);
			}
		}
	}
	SetReadyToDestroy();
}




//--------------------------------------------------------------------------------------

UBidMarketItem* UBidMarketItem::BidMarketItem(FString AuctionNumber, int32 BidPrice)
{
	UBidMarketItem* BPNode = NewObject<UBidMarketItem>();
	BPNode->AuctionNumber = AuctionNumber;
	BPNode->BidPrice = BidPrice;
	return BPNode;
}

void UBidMarketItem::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/bid"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("auctionNumber", AuctionNumber);
	_payloadJson->SetStringField("bidPrice", FString::FromInt(BidPrice));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBidMarketItem::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UBidMarketItem : %s : %s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UBidMarketItem - cannot process request"));
		Finished.Broadcast(0, FAuctionItem(), TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UBidMarketItem::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FAuctionItem item;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, item, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), item, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (res->HasField(TEXT("data")))
				item = MakeAuctionItem(res->GetObjectField(TEXT("data")));
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, item, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, item, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}


//--------------------------------------------------------------------------------------

UWatchMarketItem* UWatchMarketItem::WatchMarketItem(FString AuctionNumber, bool IsWatch)
{
	UWatchMarketItem* BPNode = NewObject<UWatchMarketItem>();
	BPNode->AuctionNumber = AuctionNumber;
	BPNode->IsWatch = IsWatch;
	return BPNode;
}

void UWatchMarketItem::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/watch"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("auctionNumber", AuctionNumber);
	_payloadJson->SetStringField("watchYN", IsWatch ? TEXT("Y") : TEXT("N"));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UWatchMarketItem::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UWatchMarketItem : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UWatchMarketItem - cannot process request"));
		Finished.Broadcast(0, FAuctionItem(), TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UWatchMarketItem::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FAuctionItem item;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, item, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), item, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				item = MakeAuctionItem(res->GetObjectField(TEXT("data")));

				Finished.Broadcast(resultCode, item, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, item, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}


//--------------------------------------------------------------------------------------

UBuyMarketItem* UBuyMarketItem::BuyMarketItem(FString AuctionNumber, int32 BuyNowPrice)
{
	UBuyMarketItem* BPNode = NewObject<UBuyMarketItem>();
	BPNode->AuctionNumber = AuctionNumber;
	BPNode->BuyNowPrice = BuyNowPrice;
	return BPNode;
}

void UBuyMarketItem::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/buyitnow"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("auctionNumber", AuctionNumber);
	_payloadJson->SetStringField("buyitNowPrice", FString::FromInt(BuyNowPrice));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBuyMarketItem::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UBuyMarketItem : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UBuyMarketItem - cannot process request"));
		Finished.Broadcast(0, FAuctionItem(), TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UBuyMarketItem::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FAuctionItem item;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, item, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), item, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (res->HasField(TEXT("data")))
				item = MakeAuctionItem(res->GetObjectField(TEXT("data")));
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, item, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, item, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}



//--------------------------------------------------------------------------------------

UGetMarketItems* UGetMarketItems::GetMarketItems(EAuctionCategory Category, EMarketSorting Sorting, int32 Page, int32 Count)
{
	UGetMarketItems* BPNode = NewObject<UGetMarketItems>();
	BPNode->Category = Category;
	BPNode->Sorting = Sorting;
	BPNode->Page = Page;
	BPNode->Count = Count;
	return BPNode;
}

void UGetMarketItems::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/list"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("page", FString::FromInt(Page));
	_payloadJson->SetStringField("showCnt", FString::FromInt(Count));

	FString TempString;
	switch (Sorting)
	{
	case EMarketSorting::BidPrice:
		TempString = TEXT("current");
		break;
	case EMarketSorting::BidCount:
		TempString = TEXT("bids");
		break;
	case EMarketSorting::WatchsCount:
		TempString = TEXT("watchs");
		break;
	default:
		TempString = TEXT("regdate");
		break;
	}
	_payloadJson->SetStringField("sortType", TempString);

	switch (Category)
	{
	case EAuctionCategory::Shirts:
		_payloadJson->SetStringField("category", TEXT("11"));
		break;
	case EAuctionCategory::Pants:
		_payloadJson->SetStringField("category", TEXT("12"));
		break;
	case EAuctionCategory::Shoes:
		_payloadJson->SetStringField("category", TEXT("13"));
		break;
	case EAuctionCategory::ETC:
		_payloadJson->SetStringField("category", TEXT("14"));
		break;
	}

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetMarketItems::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetMarketItems : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMarketItems - cannot process request"));
		TArray<FAuctionItem> Items;
		Finished.Broadcast(0, TEXT("cannot process request"), 0, Items);
		SetReadyToDestroy();
	}
}

void UGetMarketItems::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FAuctionItem> Items;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, TEXT("The Internet connection appears to be offline."), 0, Items);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), msg, 0, Items);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

				int32 totalPage = data->GetIntegerField("totalPage");

				const TArray<TSharedPtr<FJsonValue>> list = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> j : list)
				{
					Items.Add(MakeAuctionItem(j->AsObject()));
				}

				Finished.Broadcast(resultCode, TEXT(""), totalPage, Items);
			}
			else
			{
				Finished.Broadcast(resultCode, res->GetStringField("resultMessage"), 0, Items);
			}
		}
	}
	SetReadyToDestroy();
}


//--------------------------------------------------------------------------------------

UGetMyMarketItems* UGetMyMarketItems::GetMyMarketItems(EMyMarketCategory Category, int32 Page, int32 Count)
{
	UGetMyMarketItems* BPNode = NewObject<UGetMyMarketItems>();
	BPNode->Category = Category;
	BPNode->Page = Page;
	BPNode->Count = Count;
	return BPNode;
}

void UGetMyMarketItems::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/me"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("page", FString::FromInt(Page));
	_payloadJson->SetStringField("showCnt", FString::FromInt(Count));

	FString TempString;
	switch (Category)
	{
	case EMyMarketCategory::Bidding:
		TempString = TEXT("bidding");
		break;
	case EMyMarketCategory::Sale:
		TempString = TEXT("sale");
		break;
	default:
		TempString = TEXT("watch");
		break;
	}
	_payloadJson->SetStringField("selectType", TempString);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetMyMarketItems::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetMyMarketItems : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyMarketItems - cannot process request"));
		TArray<FAuctionItem> Items;
		Finished.Broadcast(0, TEXT("cannot process request"), 0, Items);
		SetReadyToDestroy();
	}
}

void UGetMyMarketItems::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FAuctionItem> Items;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, TEXT("The Internet connection appears to be offline."), 0, Items);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), msg, 0, Items);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

				int32 totalPage = data->GetIntegerField("totalPage");

				const TArray<TSharedPtr<FJsonValue>> list = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> j : list)
				{
					Items.Add(MakeAuctionItem(j->AsObject()));
				}

				Finished.Broadcast(resultCode, TEXT(""), totalPage, Items);
			}
			else
			{
				Finished.Broadcast(resultCode, res->GetStringField("resultMessage"), 0, Items);
			}
		}
	}
	SetReadyToDestroy();
}



//--------------------------------------------------------------------------------------

UGetMyWatchItem* UGetMyWatchItem::GetMyWatchItem()
{
	UGetMyWatchItem* BPNode = NewObject<UGetMyWatchItem>();
	return BPNode;
}

void UGetMyWatchItem::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/market/me/watch"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetMyWatchItem::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetMyWatchItem "));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyWatchItem - cannot process request"));
		TArray<FString> Items;
		Finished.Broadcast(false, 0, TEXT("cannot process request"), Items);
		SetReadyToDestroy();
	}
}

void UGetMyWatchItem::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FString> Items;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), Items);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyWatchItem - %s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, Items);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyWatchItem::OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

				const TArray<TSharedPtr<FJsonValue>> list = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> j : list)
				{
					Items.Add(j->AsObject()->GetStringField(TEXT("auctionNumber")));
				}

				Finished.Broadcast(true, resultCode, TEXT(""), Items);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), Items);
			}
		}
	}
	SetReadyToDestroy();
}

#pragma endregion

#pragma region Costumization

UAvatarCustomizing* UAvatarCustomizing::AvatarCustomizing()
{
	UAvatarCustomizing* BPNode = NewObject<UAvatarCustomizing>();
	return BPNode;
}

void UAvatarCustomizing::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/avatarCustomizing"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("PATCH"));
	
	HttpRequest->SetContentAsString(URM_Singleton::GetSingleton(GetWorld())->GetPlayerCharacter().ToJson());
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAvatarCustomizing::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UAvatarCustomizing : %s %s"), *HttpRequest->GetURL(), *URM_Singleton::GetSingleton(GetWorld())->GetPlayerCharacter().ToJson());
	}
	else
	{
		UE_LOG(LogRMProtocol, Log, TEXT("UAvatarCustomizing - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UAvatarCustomizing::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				
			    TSharedPtr<FJsonObject> customAvatarInfo = res->GetObjectField(TEXT("data"));
                TSharedPtr<FJsonObject> baseInfo = customAvatarInfo->GetObjectField(TEXT("baseInfo"));
                FPlayerCharacter& PlayerCharacter = URM_Singleton::GetSingleton(GetWorld())->GetPlayerCharacter();
				
				if (baseInfo->GetIntegerField("gender") == 0){PlayerCharacter.Gender = EGenderRM::Male;}
				else if (baseInfo->GetIntegerField("gender") == 1){PlayerCharacter.Gender = EGenderRM::Female;}
				else if (baseInfo->GetIntegerField("gender") == 2){PlayerCharacter.Gender = EGenderRM::Male2;}
				else if (baseInfo->GetIntegerField("gender") == 3){PlayerCharacter.Gender = EGenderRM::Female2;}
				else if (baseInfo->GetIntegerField("gender") == 4){PlayerCharacter.Gender = EGenderRM::Male3;}
				else if (baseInfo->GetIntegerField("gender") == 5){PlayerCharacter.Gender = EGenderRM::Female3;}
				else if (baseInfo->GetIntegerField("gender") == 6){PlayerCharacter.Gender = EGenderRM::Male4;}
				else if (baseInfo->GetIntegerField("gender") == 7){PlayerCharacter.Gender = EGenderRM::Female4;}
	
				PlayerCharacter.TopSize = FCString::Atof(*baseInfo->GetStringField("topSize"));
				PlayerCharacter.BottomSize = FCString::Atof(*baseInfo->GetStringField("bottomSize"));
				PlayerCharacter.CheekSize = FCString::Atof(*baseInfo->GetStringField("cheekSize"));
				PlayerCharacter.Scale = FCString::Atof(*baseInfo->GetStringField("tallScale"));
				PlayerCharacter.HeadScale = FCString::Atof(*baseInfo->GetStringField("headScale"));
				PlayerCharacter.HandScale = FCString::Atof(*baseInfo->GetStringField("handScale"));
				PlayerCharacter.SkinColor = FColor::FromHex(*baseInfo->GetStringField("skinColor"));

                TSharedPtr<FJsonObject> costumeInfo = customAvatarInfo->GetObjectField(TEXT("costumeInfo"));
                PlayerCharacter.Costume[ECostumePart::Head] = FName(*costumeInfo->GetStringField("head"));
                PlayerCharacter.Costume[ECostumePart::Hair] = FName(*costumeInfo->GetStringField("hair"));
                PlayerCharacter.Costume[ECostumePart::Top] = FName(*costumeInfo->GetStringField("top"));
                PlayerCharacter.Costume[ECostumePart::Bottom] = FName(*costumeInfo->GetStringField("bottom"));
                PlayerCharacter.Costume[ECostumePart::Shoes] = FName(*costumeInfo->GetStringField("shoes"));
                // PlayerCharacter.Costume[ECostumePart::FaceSkin] = FName(*costumeInfo->GetStringField("faceSkin"));
                //PlayerCharacter.Costume[ECostumePart::EyeBrow] = FName(*costumeInfo->GetStringField("eyeBrow"));
                //PlayerCharacter.Costume[ECostumePart::Lip] = FName(*costumeInfo->GetStringField("lip"));

                TSharedPtr<FJsonObject> eyeInfo = customAvatarInfo->GetObjectField(TEXT("eyeInfo"));
                PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::EyeColor] = FCString::Atof(*eyeInfo->GetStringField("eyeColor"));
                PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::IrisBrightness] = FCString::Atof(*eyeInfo->GetStringField("irisBrightness"));
                PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::PupilScale] = FCString::Atof(*eyeInfo->GetStringField("pupilScale"));
                PlayerCharacter.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::ScleraBrightness] = FCString::Atof(*eyeInfo->GetStringField("scleraBrightness"));

                TSharedPtr<FJsonObject> morphInfo = customAvatarInfo->GetObjectField(TEXT("morphInfo"));
				int i = 0;
				for (auto MorphTarget : PlayerCharacter.MorphTargetValue)
				{
					FString str = EnumToString(TEXT("EMorphTarget"), i);
					str[0] = std::tolower(str[0]);
					str = str.Replace(TEXT(" "), TEXT(""), ESearchCase::Type::IgnoreCase);
					PlayerCharacter.MorphTargetValue[i++].Value = FCString::Atof(*morphInfo->GetStringField(str));
				}

                FPlayerCharacter& PlayerCharacter1 = URM_Singleton::GetSingleton(GetWorld())->GetPlayerCharacter(1);
                PlayerCharacter1 = PlayerCharacter;
				
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

#pragma endregion

#pragma region Membership
UCheckAccountEnd* UCheckAccountEnd::CheckAccountEnd()
{
	UCheckAccountEnd* BPNode = NewObject<UCheckAccountEnd>();
	return BPNode;
}

void UCheckAccountEnd::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/checkAccountEndTime"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCheckAccountEnd::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("CheckAccountEnd : %s, %s"), *HttpRequest->GetURL(), *URMProtocolFunctionLibrary::GetAccessToken());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UCheckAccountEnd - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UCheckAccountEnd::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{

		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}
#pragma endregion 
#pragma region FIRST_REWARD
UFirstRewardFirebase* UFirstRewardFirebase::CheckCanGetRewardFirebase()
{
	UFirstRewardFirebase* BPNode = NewObject<UFirstRewardFirebase>();
	return BPNode;
}

void UFirstRewardFirebase::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/first-reward/firebase"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UFirstRewardFirebase::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("CheckAccountEnd : %s, %s"), *HttpRequest->GetURL(), *URMProtocolFunctionLibrary::GetAccessToken());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UFirstRewardFirebase - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UFirstRewardFirebase::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{

		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}

}

UNPCTutorialAPI* UNPCTutorialAPI::NPCTutorialAPI(int32 npcTutorial)
{
	UNPCTutorialAPI* BPNode = NewObject<UNPCTutorialAPI>();
	BPNode->npcTutorial = npcTutorial;
	return BPNode;
}

void UNPCTutorialAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/npcTutorial"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("npcTutorial", npcTutorial);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNPCTutorialAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UNPCTutorialAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT("NPCTutorialAPI Call Success"));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UNPCIdAPI* UNPCIdAPI::NPCIdAPI(int32 npcChoice)
{
	UNPCIdAPI* BPNode = NewObject<UNPCIdAPI>();
	BPNode->npcChoice = npcChoice;
	return BPNode;
}

void UNPCIdAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/npcChoice"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("npcChoice", npcChoice);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UNPCIdAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UNPCIdAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}

UActionPointAdd* UActionPointAdd::PointAdd()
{
	UActionPointAdd* BPNode = NewObject<UActionPointAdd>();
	return BPNode;
}

void UActionPointAdd::Activate()
{
	Super::Activate();

	FActionPoint ActionPoint;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/action-point/add"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));
	
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UActionPointAdd::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), ActionPoint);
		SetReadyToDestroy();
	}
}

void UActionPointAdd::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FActionPoint ActionPoint;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast( 0, false, TEXT("The Internet connection appears to be offline."), ActionPoint);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(),false,  msg, ActionPoint);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TSharedPtr<FJsonObject> point = data->GetObjectField(TEXT("point"));
				ActionPoint.point = point->GetIntegerField("point");
				ActionPoint.updatedAt = point->GetStringField("updatedAt");
				

				Finished.Broadcast(resultCode, true, TEXT(""), ActionPoint);
			}
			else
			{
				Finished.Broadcast( resultCode, false, res->GetStringField("resultMessage"), ActionPoint);
			}
		}
	}
	SetReadyToDestroy();
}

UActionPointUse* UActionPointUse::PointUse()
{
	UActionPointUse* BPNode = NewObject<UActionPointUse>();
	return BPNode;
}

void UActionPointUse::Activate()
{
	Super::Activate();

	FActionPoint ActionPoint;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/action-point/used"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));
	
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UActionPointUse::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), ActionPoint);
		SetReadyToDestroy();
	}
}

void UActionPointUse::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FActionPoint ActionPoint;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ActionPoint);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast( Response->GetResponseCode(), false, msg, ActionPoint);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TSharedPtr<FJsonObject> point = data->GetObjectField(TEXT("point"));
				ActionPoint.point = point->GetIntegerField("point");
				ActionPoint.updatedAt = point->GetStringField("updatedAt");

				Finished.Broadcast( resultCode ,true,  TEXT(""), ActionPoint);
			}
			else
			{
				Finished.Broadcast( resultCode,  false,res->GetStringField("resultMessage"), ActionPoint);
			}
		}
	}
	SetReadyToDestroy();
}

UTutorialAPI* UTutorialAPI::TutorialAPI(FString type, int32 val)
{
	UTutorialAPI* BPNode = NewObject<UTutorialAPI>();
	BPNode->type = type;
	BPNode->val = val;
	return BPNode;
}

void UTutorialAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/user/tutorial"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("type", type);
	_payloadJson->SetNumberField("val", val);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTutorialAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UTutorialAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}
#pragma endregion

#pragma region RM_EVENT
void GetEventInfoByJsonObject(TSharedPtr<FJsonObject> jsonObject, FEventInfo& info)
{
	info.eventSeq = jsonObject->GetIntegerField(TEXT("eventSeq"));
	info.eventCode = jsonObject->GetStringField(TEXT("eventCode"));
	info.title = jsonObject->GetStringField(TEXT("title"));
	info.bUseLimitDate = jsonObject->GetStringField(TEXT("dateYN")) == TEXT("Y") ? true : false;
	if (info.bUseLimitDate)
	{
		// Get LimitDay Info
		FDateTime::Parse(jsonObject->GetStringField("startDt"), info.startDay);
		FDateTime::Parse(jsonObject->GetStringField("endDt"), info.endDay);
	}
	info.limitDays = jsonObject->GetIntegerField(TEXT("limitDays"));
	info.bIsAttendable = jsonObject->GetStringField(TEXT("attendYN")) == TEXT("Y") ? true : false;
	info.sortNum = jsonObject->GetIntegerField(TEXT("sortNum"));
	info.bIsNew = jsonObject->GetStringField(TEXT("isNew")) == TEXT("Y") ? true : false;

	int type = jsonObject->GetIntegerField(TEXT("type"));
	switch (type)
	{
		case 0:  info.eventType = EEventType::Normal;	break;
		case 1:  info.eventType = EEventType::Random;	break;
		case 2:  info.eventType = EEventType::Consume;	break;
		default: info.eventType = EEventType::Normal;	break;
	}

	info.bIsConsumable = jsonObject->GetStringField(TEXT("consumeYN")) == TEXT("Y") ? true : false;
	info.consumeCount = jsonObject->GetIntegerField(TEXT("consumeCnt"));
	info.consumeMax = jsonObject->GetIntegerField(TEXT("consumeMax"));

	info.limitPerOneDay = jsonObject->GetStringField(TEXT("onedayYN")) == TEXT("Y") ? true : false;
	info.totalAttendCount = jsonObject->GetIntegerField(TEXT("totalAttendCnt"));
	info.myTotalAttendCount = jsonObject->GetIntegerField(TEXT("isMeAttendCnt"));

	info.baseCoin = jsonObject->GetIntegerField(TEXT("baseCoin"));
	info.afterCoin = jsonObject->GetIntegerField(TEXT("afterCoin"));
	info.bIsOnlyOne = jsonObject->GetStringField(TEXT("onlyOne")) == TEXT("Y") ? true : false;

	info.attendCoin = jsonObject->GetIntegerField(TEXT("attendCoin"));

	// 기간 이벤트인데 이벤트 기간이 지났으면 'IsNew' 를 강제로 false 설정함
	/*if (info.bUseLimitDate && info.endDay < FDateTime::UtcNow())
	{
		info.bIsNew = false;
	}*/
}


UGetEventList* UGetEventList::GetEventList(UObject* WorldContextObject)
{
	UGetEventList* BPNode = NewObject<UGetEventList>();
	UWorld* world = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	if (world)
	{
		BPNode->RMEventSubSystem = UGameplayStatics::GetGameInstance(world)->GetSubsystem<URMEventSubSystem>();
	}
	return BPNode;
}

void UGetEventList::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/event/list"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetEventList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetEventList : %s, %s"), *HttpRequest->GetURL(), *URMProtocolFunctionLibrary::GetAccessToken());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetEventList - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UGetEventList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TMap<int, FEventInfo> eventInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{

		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
			const TArray<TSharedPtr<FJsonValue>> list = data->GetArrayField(TEXT("currentEventInfoList"));
			int priority = 1;
			for (TSharedPtr<FJsonValue> valJson : list)
			{
				TSharedPtr<FJsonObject> val = valJson->AsObject();
				FEventInfo info;
				info.priority = priority;
				GetEventInfoByJsonObject(val, info);
				eventInfo.Add(info.eventSeq, info);
				priority++;
			}

			if (resultCode == 200)
			{
				/* Setting RMEventSubSystem::EventInfo */
				RMEventSubSystem->SetEventInfo(eventInfo);
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
		SetReadyToDestroy();
	}
}

UAttendEvent* UAttendEvent::AttendEvent(UObject* WorldContextObject, int eventSeq, FString val, FString val2)
{
	UAttendEvent* BPNode = NewObject<UAttendEvent>();
	BPNode->eventSeq = eventSeq;
	BPNode->Val_1 = val.IsEmpty() ? TEXT("") : val;
	BPNode->Val_2 = val2.IsEmpty() ? TEXT("") : val2;
	UWorld* world = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	if (world)
		BPNode->RMEventSubSystem = UGameplayStatics::GetGameInstance(world)->GetSubsystem<URMEventSubSystem>();

	return BPNode;
}

void UAttendEvent::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/event/attend"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("eventSeq", FString::FromInt(eventSeq));
	_payloadJson->SetStringField("val", Val_1);
	_payloadJson->SetStringField("val_2", Val_2);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAttendEvent::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UAttendEvent : %s"), *HttpRequest->GetURL());
	}
	else
	{
		FMyEventAttendanceInfo myInfo;
		UE_LOG(LogRMProtocol, Warning, TEXT("UAttendEvent - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), myInfo);
		SetReadyToDestroy();
	}
}

void UAttendEvent::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FMyEventAttendanceInfo myInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), myInfo);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, myInfo);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{

		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
			// Get Event Info
			TSharedPtr<FJsonObject> val  = data->GetObjectField(TEXT("eventInfo"));
			FEventInfo info;
			GetEventInfoByJsonObject(val, info);

			RMEventSubSystem->ModifyEventInfo(info);
			/* 하루에 한 번만 참여하는 이벤트면 클라 자체에서 bIsNew를 False처리한다. */
			/*if ((info.limitPerOneDay || info.bIsOnlyOne) && RMEventSubSystem != nullptr)
			{
				RMEventSubSystem->SetIsNewFlagForce_Internal(info.eventSeq, false);
				UE_LOG(LogTemp, Warning, TEXT("Force modified bIsNew"));
			}*/

			// Get My Attendance Info
			TSharedPtr<FJsonObject> val2 = data->GetObjectField(TEXT("myAttendInfo"));
			myInfo.eventSeq = val2->GetIntegerField(TEXT("eventSeq"));
			myInfo.eventCode = val2->GetStringField(TEXT("eventCode"));
			myInfo.userSeq = val2->GetIntegerField(TEXT("userSeq"));
			FDateTime::Parse(val2->GetStringField(TEXT("regdate")), myInfo.regDate);
			myInfo.returnValue = val2->GetStringField(TEXT("val"));
			myInfo.attendTotalCount = val2->GetIntegerField(TEXT("attendTotalCnt"));

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""), myInfo);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), myInfo);
			}
		}
		SetReadyToDestroy();
	}
}

UEventWinner* UEventWinner::EventWinner(UObject* WorldContextObject, int eventSeq, FString Value, FString Value2)
{
	UEventWinner* BPNode = NewObject<UEventWinner>();
	BPNode->eventSeq = eventSeq;
	BPNode->Value = Value;
	BPNode->Value2 = Value2;
	UWorld* world = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	if (world)
		BPNode->RMEventSubSystem = UGameplayStatics::GetGameInstance(world)->GetSubsystem<URMEventSubSystem>();

	return BPNode;
}

void UEventWinner::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/ticket/winner"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("eventSeq", FString::FromInt(eventSeq));
	if (!Value.IsEmpty())
	{
		HttpRequest->SetVerb(TEXT("PATCH"));
		_payloadJson->SetStringField("val", Value);
		_payloadJson->SetStringField("val_2", Value2);
	}
	
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEventWinner::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UEventWinner : %s"), *HttpRequest->GetURL());
	}
	else
	{
		FEventWinnerInfo EventWinnerInfo;
		UE_LOG(LogRMProtocol, Warning, TEXT("UEventWinner - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), EventWinnerInfo);
		SetReadyToDestroy();
	}
}

void UEventWinner::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FEventWinnerInfo EventWinnerInfo;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), EventWinnerInfo);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, EventWinnerInfo);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

				if (data->HasField("giveCoin"))
					EventWinnerInfo.GiveCoin = data->GetIntegerField("giveCoin");

				
				if (data->HasField("winner"))
				{
					// 이벤트 당점자일 경우
					TSharedPtr<FJsonObject> winner = data->GetObjectField("winner");
					
					if (winner->HasField("eventSeq"))
						EventWinnerInfo.EventSeq = winner->GetIntegerField("eventSeq");

					if (winner->HasField("eventCode"))
						EventWinnerInfo.EventCode = winner->GetStringField("eventCode");

					if (winner->HasField("userSeq"))
						EventWinnerInfo.UserSeq = winner->GetIntegerField("userSeq");

					if (winner->HasField("regdate"))
						FDateTime::Parse(winner->GetStringField("regdate"), EventWinnerInfo.RegDate);

					if (winner->HasField("rank"))
						EventWinnerInfo.Rank = winner->GetIntegerField("rank");

					if (winner->HasField("attendTotalCnt"))
						EventWinnerInfo.TotalAttendCount = winner->GetIntegerField("attendTotalCnt");

					if (winner->HasField("val"))
						EventWinnerInfo.Val = winner->GetStringField("val");

					if (winner->HasField("val_2"))
						EventWinnerInfo.Val2 = winner->GetStringField("val_2");
				}
				else
				{
					const TArray<TSharedPtr<FJsonValue>> winnerList = data->GetArrayField(TEXT("winnerList"));
					for (TSharedPtr<FJsonValue> list : winnerList)
					{
						TSharedPtr<FJsonObject> winner = list->AsObject();
						FEventWinnerUserInfo info;
						info.eventSeq = winner->GetIntegerField("eventSeq");
						info.userSeq = winner->GetIntegerField("userSeq");
						info.nickName = winner->GetStringField("nickName");
						info.rank = winner->GetIntegerField("rank");
						EventWinnerInfo.WinnersUserInfo.Add(info);
					}
				}
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""), EventWinnerInfo);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), EventWinnerInfo);
			}
		}
	}
}

#pragma endregion


#pragma region RM_NOTICE
UGetNoticeBoardList* UGetNoticeBoardList::GetNoticeBoardList(int32 ShowPage, int32 CountOnce, FString SortCountryCode,
	ENoticeBoardType boardType)
{
	UGetNoticeBoardList* BPNode = NewObject<UGetNoticeBoardList>();
	BPNode->page = ShowPage;
	BPNode->showCount = CountOnce;
	BPNode->countryCode = SortCountryCode;
	BPNode->noticeBoardtype = boardType;
	return BPNode;
}

void UGetNoticeBoardList::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/info/board"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("page", FString::FromInt(page));
	_payloadJson->SetStringField("showCnt", FString::FromInt(showCount));
	_payloadJson->SetStringField("sortType", countryCode.IsEmpty() ? TEXT("EN") : countryCode);
	FString str = EnumToString(TEXT("ENoticeBoardType"), static_cast<int32>(noticeBoardtype)).ToLower();
	_payloadJson->SetStringField("keyword", str);
	
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetNoticeBoardList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UAttendEvent : %s"), *HttpRequest->GetURL());
	}
	else
	{
		TArray<FNoticeBoardInfo> Infos;
		UE_LOG(LogRMProtocol, Warning, TEXT("UAttendEvent - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), 0, 0, Infos);
		SetReadyToDestroy();
	}
}

void UGetNoticeBoardList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FNoticeBoardInfo> BoardListInfos;
	int totalPage = 0;
	int currentPage = 0;
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), 0, 0, BoardListInfos);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, 0, 0, BoardListInfos);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMsg = res->GetStringField("resultMessage");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				totalPage	= data->GetIntegerField("totalPage");
				currentPage = data->GetIntegerField("currentPage");
				
				TArray<TSharedPtr<FJsonValue>> listArr = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> item : listArr)
				{
					TSharedPtr<FJsonObject> obj = item->AsObject();
					if (obj)
					{
						FNoticeBoardInfo info;
						info.boardSeq =		(obj->HasField("seq"))		?	obj->GetIntegerField("seq") 	: 	-1;
						info.boardCode =	(obj->HasField("code"))		?	obj->GetStringField("code") 	: 	TEXT(""); 						
						info.countryCode =	(obj->HasField("country"))	?	obj->GetStringField("country")	: 	TEXT("EN");
						info.title =		(obj->HasField("title"))	?	obj->GetStringField("title")	: 	TEXT("");
						info.subTitle =		(obj->HasField("subtitle")) ?	obj->GetStringField("subtitle") : 	TEXT("");
						info.URL =			(obj->HasField("url"))		?	obj->GetStringField("url")		: 	TEXT("");

						if (obj->HasField("regdate"))
							FDateTime::Parse(obj->GetStringField("regdate"), info.regDate);
						
						BoardListInfos.Add(info);
					}
				}
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""), totalPage, currentPage, BoardListInfos);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), totalPage, currentPage, BoardListInfos);
			}
		}
	}
}
#pragma endregion 


#pragma region PostBase64ImageToServer

UPostBase64ImageToServer* UPostBase64ImageToServer::PostBase64ImageToServer(const FString& Base64)
{
	UPostBase64ImageToServer* BPNode = NewObject<UPostBase64ImageToServer>();
	BPNode->Base64 = Base64;
	return BPNode;
}

void UPostBase64ImageToServer::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/qatar-photo/new/image"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

	_payloadJson->SetStringField("image", Base64);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostBase64ImageToServer::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("AttendanceReward : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPostBase64DataToServer - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"), "", "");
		SetReadyToDestroy();
	}
}

void UPostBase64ImageToServer::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), "", "");
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		// Finished.Broadcast(Response->GetResponseCode(), false, msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, "", "");
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			FString imageUrl = res->GetObjectField("data")->GetStringField("imageUrl");
			FString thumbnailUrl = res->GetObjectField("data")->GetStringField("thumbnailUrl");
			Finished.Broadcast(true, resultCode, TEXT(""), imageUrl, thumbnailUrl);
		}
		else
		{
			Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), "", "");
		}
	}
	SetReadyToDestroy();
}

#pragma endregion


#pragma region PostRegistPhotoData
UPostRegistPhotoData* UPostRegistPhotoData::PostRegistPhotoData(const FString& imageUrl, int frameNumber)
{
	UPostRegistPhotoData* BPNode = NewObject<UPostRegistPhotoData>();
	BPNode->ImageUrl = imageUrl;
	BPNode->FrameNumber = frameNumber;
	return BPNode;
}

void UPostRegistPhotoData::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/qatar-photo/new/data"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

	_payloadJson->SetStringField("imageUrl", ImageUrl);
	_payloadJson->SetNumberField("frameNumber", FrameNumber);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostRegistPhotoData::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("AttendanceReward : %s\n%s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPostRegistPhotoData - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"), 0);
		SetReadyToDestroy();
	}
}

void UPostRegistPhotoData::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), 0);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), TEXT("The Internet connection appears to be offline."), 0);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}
		int resultCode = res->GetIntegerField("resultCode");
		FString resultMessage = res->GetObjectField("data")->GetStringField("resultMessage");
		int sessionId = res->GetObjectField("data")->GetNumberField("sessionId");
		Finished.Broadcast(resultCode == 200, resultCode, resultMessage, sessionId);
	}
	SetReadyToDestroy();
}

#pragma endregion


#pragma region GetPhotoList

UGetPhotoList* UGetPhotoList::GetPhotoList(int32 Page, int32 Limit)
{	
	UGetPhotoList* BPNode = NewObject<UGetPhotoList>();
	BPNode->Page = Page;
	BPNode->Limit = Limit;
	return BPNode;
}

void UGetPhotoList::Activate()
{
	TArray<FPhotoInfo> PhotoInfos;
	int TotalCount = 0;
	int LastPage = 0;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	FString SearchParams = TEXT("?page=") + FString::FromInt(Page) + TEXT("&limit=") + FString::FromInt(Limit);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/qatar-photo/list%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *SearchParams));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetPhotoList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetPhotoList : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetPhotoList - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"), PhotoInfos, TotalCount, LastPage);
		SetReadyToDestroy();
	}
}

void UGetPhotoList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FPhotoInfo> PhotoInfos;
	int TotalCount = 0;
	int LastPage = 0;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), PhotoInfos, TotalCount, LastPage);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, PhotoInfos, TotalCount, LastPage);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMsg = res->GetStringField("resultMessage");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TSharedPtr<FJsonObject> pagination = data->GetObjectField(TEXT("pagination"));

				TotalCount = pagination->GetIntegerField(TEXT("total"));
				LastPage = pagination->GetIntegerField(TEXT("lastPage"));

				TArray<TSharedPtr<FJsonValue>> listArr = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> item : listArr)
				{
					TSharedPtr<FJsonObject> obj = item->AsObject();
					if (obj)
					{
						FPhotoInfo info;
						info.Seq = (obj->HasField("seq")) ? obj->GetIntegerField("seq") : -1;
						info.UserNickname = (obj->HasField("userNickname")) ? obj->GetStringField("userNickname") : TEXT("");
						info.ImageUrl = (obj->HasField("imageUrl")) ? obj->GetStringField("imageUrl") : TEXT("");
						info.ThumbnailUrl = (obj->HasField("thumbnailUrl")) ? obj->GetStringField("thumbnailUrl") : TEXT("");
						info.FrameNumber = (obj->HasField("frameNumber")) ? obj->GetIntegerField("frameNumber") : 0;
						info.CountLike = (obj->HasField("countLike")) ? obj->GetIntegerField("countLike") : 0;
						info.IsSelected = (obj->HasField("isSelected")) ? obj->GetBoolField("isSelected") : false;
						//info.ReportCount = (obj->HasField("reportCount")) ? obj->GetIntegerField("reportCount") : 0;
						info.IsReported = (obj->HasField("isReported")) ? obj->GetBoolField("isReported") : false;

						PhotoInfos.Add(info);
					}
				}
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(true, resultCode, TEXT(""), PhotoInfos, TotalCount, LastPage);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), PhotoInfos, TotalCount, LastPage);
			}
		}
	}
}
#pragma endregion


#pragma region PostPhotoLike

UPostPhotoLike* UPostPhotoLike::PostPhotoLike(int32 Seq)
{
	UPostPhotoLike* BPNode = NewObject<UPostPhotoLike>();
	BPNode->Seq = Seq;
	return BPNode;
}

void UPostPhotoLike::Activate()
{
	bool IsSelected = false; 
	int32 CountLike = 0;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/qatar-photo/like"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

	//_payloadJson->SetStringField("image", Base64String);
	_payloadJson->SetNumberField("seq", Seq);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostPhotoLike::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("AttendanceReward : %s\n%s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPostPhotoLike - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"), IsSelected, CountLike);
		SetReadyToDestroy();
	}
}

void UPostPhotoLike::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	bool IsSelected = false;
	int32 CountLike = 0;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), IsSelected, CountLike);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		// Finished.Broadcast(Response->GetResponseCode(), false, msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, IsSelected, CountLike);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}

		int resultCode = res->GetIntegerField("resultCode");

		TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));

		IsSelected = data->GetBoolField("isSelected");
		CountLike = data->GetIntegerField("countLike");

		if (resultCode == 200)
		{
			Finished.Broadcast(true, resultCode, TEXT(""), IsSelected, CountLike);
		}
		else
		{
			Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), IsSelected, CountLike);
		}
	}
	SetReadyToDestroy();
}

#pragma endregion


#pragma region PostPhotoReport

UPostPhotoReport* UPostPhotoReport::PostPhotoReport(int32 Seq)
{
	UPostPhotoReport* BPNode = NewObject<UPostPhotoReport>();
	BPNode->Seq = Seq;
	return BPNode;
}

void UPostPhotoReport::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/qatar-photo/report"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

	//_payloadJson->SetStringField("image", Base64String);
	_payloadJson->SetNumberField("seq", Seq);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostPhotoReport::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("AttendanceReward : %s\n%s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPostPhotoReport - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UPostPhotoReport::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		// Finished.Broadcast(Response->GetResponseCode(), false, msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}

		int resultCode = res->GetIntegerField("resultCode");

		if (resultCode == 200)
		{
			Finished.Broadcast(true, resultCode, TEXT(""));
		}
		else
		{
			Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"));
		}
	}
	SetReadyToDestroy();
}

#pragma endregion


#pragma region GetPhotoRankListOfWeek

UGetPhotoRankListOfWeek* UGetPhotoRankListOfWeek::GetPhotoRankListOfWeek()
{
	UGetPhotoRankListOfWeek* BPNode = NewObject<UGetPhotoRankListOfWeek>();
	return BPNode;
}

void UGetPhotoRankListOfWeek::Activate()
{
	TArray<FPhotoInfo> PhotoInfos;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/qatar-photo/ranking/week"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetPhotoRankListOfWeek::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetPhotoRankListOfWeek : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetPhotoRankListOfWeek - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"), PhotoInfos);
		SetReadyToDestroy();
	}
}

void UGetPhotoRankListOfWeek::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FPhotoInfo> PhotoInfos;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), PhotoInfos);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, PhotoInfos);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMsg = res->GetStringField("resultMessage");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TSharedPtr<FJsonObject> pagination = data->GetObjectField(TEXT("pagination"));

				TArray<TSharedPtr<FJsonValue>> listArr = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> item : listArr)
				{
					TSharedPtr<FJsonObject> obj = item->AsObject();
					if (obj)
					{
						FPhotoInfo info;
						info.Seq = (obj->HasField("seq")) ? obj->GetIntegerField("seq") : -1;
						info.UserNickname = (obj->HasField("userNickname")) ? obj->GetStringField("userNickname") : TEXT("");
						info.ImageUrl = (obj->HasField("imageUrl")) ? obj->GetStringField("imageUrl") : TEXT("");
						info.ThumbnailUrl = (obj->HasField("thumbnailUrl")) ? obj->GetStringField("thumbnailUrl") : TEXT("");
						info.FrameNumber = (obj->HasField("frameNumber")) ? obj->GetIntegerField("frameNumber") : 0;
						info.CountLike = (obj->HasField("countLike")) ? obj->GetIntegerField("countLike") : 0;
						info.IsSelected = (obj->HasField("isSelected")) ? obj->GetBoolField("isSelected") : false;
						info.IsReported = (obj->HasField("isReported")) ? obj->GetBoolField("isReported") : false;

						PhotoInfos.Add(info);
					}
				}
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(true, resultCode, TEXT(""), PhotoInfos);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), PhotoInfos);
			}
		}
	}
}
#pragma endregion


#pragma region GetPhotoRankListOfWhole

UGetPhotoRankListOfWhole* UGetPhotoRankListOfWhole::GetPhotoRankListOfWhole()
{
	UGetPhotoRankListOfWhole* BPNode = NewObject<UGetPhotoRankListOfWhole>();
	return BPNode;
}

void UGetPhotoRankListOfWhole::Activate()
{
	TArray<FPhotoInfo> PhotoInfos;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/qatar-photo/ranking/whole"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetPhotoRankListOfWhole::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UGetPhotoRankListOfWhole : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetPhotoRankListOfWhole - cannot process request"));
		Finished.Broadcast(false, 0, TEXT("cannot process request"), PhotoInfos);
		SetReadyToDestroy();
	}

}

void UGetPhotoRankListOfWhole::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FPhotoInfo> PhotoInfos;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(false, 0, TEXT("The Internet connection appears to be offline."), PhotoInfos);
		SetReadyToDestroy();
		return;

		 
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(false, Response->GetResponseCode(), msg, PhotoInfos);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMsg = res->GetStringField("resultMessage");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TSharedPtr<FJsonObject> pagination = data->GetObjectField(TEXT("pagination"));

				TArray<TSharedPtr<FJsonValue>> listArr = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> item : listArr)
				{
					TSharedPtr<FJsonObject> obj = item->AsObject();
					if (obj)
					{
						FPhotoInfo info;
						info.Seq = (obj->HasField("seq")) ? obj->GetIntegerField("seq") : -1;
						info.UserNickname = (obj->HasField("userNickname")) ? obj->GetStringField("userNickname") : TEXT("");
						info.ImageUrl = (obj->HasField("imageUrl")) ? obj->GetStringField("imageUrl") : TEXT("");
						info.ThumbnailUrl = (obj->HasField("thumbnailUrl")) ? obj->GetStringField("thumbnailUrl") : TEXT("");
						info.FrameNumber = (obj->HasField("frameNumber")) ? obj->GetIntegerField("frameNumber") : 0;
						info.CountLike = (obj->HasField("countLike")) ? obj->GetIntegerField("countLike") : 0;
						info.IsSelected = (obj->HasField("isSelected")) ? obj->GetBoolField("isSelected") : false;
						info.IsReported = (obj->HasField("isReported")) ? obj->GetBoolField("isReported") : false;

						PhotoInfos.Add(info);
					}
				}
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(true, resultCode, TEXT(""), PhotoInfos);
			}
			else
			{
				Finished.Broadcast(false, resultCode, res->GetStringField("resultMessage"), PhotoInfos);
			}
		}
	}
}
#pragma endregion
