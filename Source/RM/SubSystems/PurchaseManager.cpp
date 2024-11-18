#include "PurchaseManager.h"

#include "HttpModule.h"
#include "RM/RMProtocolFunctionLibrary.h"
#include"RM/RM_Singleton.h"
#include "Http.h"

FString UPurchaseManager::ParsingJson(const FString& Value)
{
	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Value);
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	FJsonSerializer::Deserialize(Reader, JsonObject);

	FString Result = JsonObject->GetStringField("receiptData");

	return Result;
}


UPurchaseAPI* UPurchaseAPI::PurchaseGoogle(const FString& receiptData)
{
	UPurchaseAPI* BPNode = NewObject<UPurchaseAPI>();
	BPNode->receiptData = receiptData;
	BPNode->TargetSeq = 0;
	return BPNode;
}

UPurchaseAPI* UPurchaseAPI::GiftGoogle(const FString& receiptData, int32 TargetSeq)
{
	UPurchaseAPI* BPNode = NewObject<UPurchaseAPI>();
	BPNode->receiptData = receiptData;
	BPNode->TargetSeq = TargetSeq;
	return BPNode;
}

void UPurchaseAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/purchase/google"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("receiptData", *receiptData);
	
	if (TargetSeq != 0)
		_payloadJson->SetStringField("targetUserSeq", FString::FromInt(TargetSeq));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPurchaseAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UPurchaseGoogleAPI : %s : %s"), *HttpRequest->GetURL(), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPurchaseGoogleAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UPurchaseAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
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
				if (data->HasField("coin"))
					FDateTime::Parse(data->GetStringField("accountEndTime"), URM_Singleton::GetSingleton(GetWorld())->TimeInfo.AccountEndTime);
				
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

UPurchaseListAPI* UPurchaseListAPI::GetPurchaseList(const FString& ItemType)
{
	UPurchaseListAPI* BPNode = NewObject<UPurchaseListAPI>();
	BPNode->ItemType = ItemType;
	return BPNode;
}

void UPurchaseListAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/info/coin/%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *ItemType));

	{ //Server Address Check
		const FString& URL = FString::Printf(TEXT("%s/info/coin/%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *ItemType);
		GEngine->AddOnScreenDebugMessage(1, 45.0f, FColor::Magenta, FString(URL));
	}
	
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));
	TArray<FCoinInfo> CoinInfos;
	CoinInfos.Empty();

	if(HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPurchaseListAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UPurchaseListAPI : %s, %s"), *HttpRequest->GetURL(), *URMProtocolFunctionLibrary::GetAccessToken());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UPurchaseList - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), CoinInfos);
		SetReadyToDestroy();
	}
}

void UPurchaseListAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TArray<FCoinInfo> CoinInfos;
		CoinInfos.Empty();
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), CoinInfos);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		TArray<FCoinInfo> CoinInfos;
		CoinInfos.Empty();
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, CoinInfos);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		TArray<FCoinInfo> CoinInfos;
		CoinInfos.Empty();

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
					TArray<TSharedPtr<FJsonValue>> ResultArray =  res->GetArrayField("data");
					for (auto Elem : ResultArray)
					{
						TSharedPtr<FJsonObject> Info = Elem->AsObject();
						FCoinInfo ArrayElem;
						ArrayElem.infoCoinSeq	= Info->GetIntegerField("infoCoinSeq");
						ArrayElem.type			= Info->GetStringField("type");
						ArrayElem.inAppId		= Info->GetStringField("inAppId");
						ArrayElem.unit			= Info->GetIntegerField("unit");
						ArrayElem.amt			= Info->GetIntegerField("amt");

						CoinInfos.Add(ArrayElem);
					}

					Finished.Broadcast(resultCode, true, TEXT(""), CoinInfos);
				}
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), CoinInfos);
			}
		}
	}
	SetReadyToDestroy();
}

UCardPackListAPI* UCardPackListAPI::GetCardPackListInfo()
{
	UCardPackListAPI* BPNode = NewObject<UCardPackListAPI>();
	return BPNode;
}

void UCardPackListAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/pack-list"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	TArray<FCardPackInfo> ResultItems;
	ResultItems.Empty();

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCardPackListAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UCardPackListAPI "));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UCardPackListAPI - cannot process request"));
		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("cannot process request"), ResultItems);
		SetReadyToDestroy();
	}
}

void UCardPackListAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TArray<FCardPackInfo> ResultItems;

		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ResultItems);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		TArray<FCardPackInfo> ResultItems;
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

		TArray<FCardPackInfo> ResultItems;
		ResultItems.Empty();

		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMSG = res->GetStringField("resultMessage");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TArray<TSharedPtr<FJsonValue>> ResultArray = data->GetArrayField("cardPackList");
				for (auto Elem : ResultArray)
				{
					const TSharedPtr<FJsonObject> Info = Elem->AsObject();
					FCardPackInfo ArrayElem;
					ArrayElem.infoSeq = Info->GetIntegerField("infoSeq");
					ArrayElem.gachaPackId = Info->GetStringField("gachaPackId");
					ArrayElem.name = Info->GetStringField("name");
					ArrayElem.type = Info->GetStringField("type");
					ArrayElem.coin = Info->GetIntegerField("coin");

					ResultItems.Add(ArrayElem);
				}

				Finished.Broadcast(resultCode, true, TEXT(""), ResultItems);
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, resultMSG, ResultItems);
			}
			else
			{
				Finished.Broadcast(resultCode, false, resultMSG, ResultItems);
			}
		}
	}
	SetReadyToDestroy();
}

UBuyGoldAPI* UBuyGoldAPI::BuyGold(const FName& GoldKey)
{
	UBuyGoldAPI* BPNode = NewObject<UBuyGoldAPI>();
	BPNode->GoldKey = GoldKey.ToString();
	return BPNode;
}

void UBuyGoldAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/buy/gold/%s"), *URMProtocolFunctionLibrary::GetBaseUrl(), *GoldKey));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBuyGoldAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("BuyGold : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UBuyGoldAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UBuyGoldAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
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

UCostumeListAPI* UCostumeListAPI::GetCostumeList()
{
	UCostumeListAPI* BPNode = NewObject<UCostumeListAPI>();
	return BPNode;
}

void UCostumeListAPI::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/info/costumes"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	TArray<FCostumeInfoList> ResultItems;
	ResultItems.Empty();

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCostumeListAPI::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetUserInfo"));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetUserInfoAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), ResultItems);
		SetReadyToDestroy();
	}
}

void UCostumeListAPI::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TArray<FCostumeInfoList> ResultItems;
		ResultItems.Empty();
		
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ResultItems);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		TArray<FCostumeInfoList> ResultItems;
		ResultItems.Empty();
		
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, ResultItems);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		TArray<FCostumeInfoList> ResultItems;
		ResultItems.Empty();
		
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMSG = res->GetStringField("resultMessage");

			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				TArray<TSharedPtr<FJsonValue>> ResultArray = data->GetArrayField("data");
				for (auto Elem : ResultArray)
				{
					const TSharedPtr<FJsonObject> Info = Elem->AsObject();
					FCostumeInfoList Array;
					Array.ItemID = data->GetStringField("ItemID");
					Array.Name = data->GetStringField("name");
					Array.Coin = data->GetIntegerField("coin");

					ResultItems.Add(Array);
				}

				Finished.Broadcast(resultCode, true, TEXT(""), ResultItems);
			}

			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, resultMSG, ResultItems);
			}
			else
			{
				Finished.Broadcast(resultCode, false, resultMSG, ResultItems);
			}
		}
	}
	SetReadyToDestroy();
}
