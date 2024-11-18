// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGameAPIManager.h"
#include "HttpModule.h"
#include "RM/RMProtocolFunctionLibrary.h"
#include"RM/RM_Singleton.h"
#include "Http.h"

UGetMyCardList* UGetMyCardList::GetMyCardList()
{
	UGetMyCardList* BPNode = NewObject<UGetMyCardList>();
	return BPNode;
}

void UGetMyCardList::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/my-list"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	TArray<FCardListElem> ResultItems;
	ResultItems.Empty();

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetMyCardList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetMyCardList "));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyWatchItem - cannot process request"));
		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("cannot process request"), ResultItems,0);
		SetReadyToDestroy();
	}
}

void UGetMyCardList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TArray<FCardListElem> ResultItems;

		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ResultItems,0);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		TArray<FCardListElem> ResultItems;
		ResultItems.Empty();

		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");

			if (resultCode == 200 || resultCode == 820)
			{
				if (res->HasField(TEXT("data")))
				{
					auto ResultArray = res->GetObjectField("data")->GetArrayField("cardList");
					for (auto Elem : ResultArray)
					{
						const TSharedPtr<FJsonObject> Info = Elem->AsObject();
						FCardListElem ArrayElem;
						ArrayElem.seq = Info->GetIntegerField("seq");
						ArrayElem.id = Info->GetIntegerField("id");
						ArrayElem.cardName = Info->GetStringField("cardName");
						ArrayElem.quantity = Info->GetIntegerField("quantity");
						ArrayElem.grade = Info->GetStringField("grade");
						ArrayElem.level = Info->GetIntegerField("lv");
						ArrayElem.position = Info->GetStringField("position");
						ArrayElem.power = Info->GetIntegerField("power");
						ArrayElem.club = Info->GetStringField("club");
						ArrayElem.contry = Info->GetStringField("country");
						ArrayElem.season = Info->GetStringField("season");
						ArrayElem.userSeq = Info->GetIntegerField("userSeq");
						ArrayElem.uid = Info->GetStringField("uid");
						ArrayElem.iGrade = Info->GetIntegerField("igrade");

						ResultItems.Add(ArrayElem);
					}

					float Trophyboost = static_cast<float>(res->GetObjectField("data")->GetNumberField("trophyBoost"));

					Finished.Broadcast(resultCode, true, TEXT(""), ResultItems, Trophyboost);
				}
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), ResultItems,0);
			}
		}
	}
	SetReadyToDestroy();
}
UPostGachaCardPack* UPostGachaCardPack::PostGachaCardPack(FString gachaPackID)
{
	UPostGachaCardPack* BPNode = NewObject<UPostGachaCardPack>();
	BPNode->gachaPackID = gachaPackID;

	return BPNode;
}

void UPostGachaCardPack::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/gacha"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	FGachaResult GachaResult;

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("gachaPackId", gachaPackID);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostGachaCardPack::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), GachaResult);
		SetReadyToDestroy();
	}
}

void UPostGachaCardPack::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	FGachaResult GachaResult;

	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."),GachaResult);
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
			if (resultCode == 200 || resultCode == 820)
			{
				if (res->HasField(TEXT("data")))
				{
					auto ResultCard = res->GetObjectField("data")->GetObjectField("card");
					GachaResult.seq = ResultCard->GetIntegerField("seq");
					GachaResult.cardDataSeq = ResultCard->GetIntegerField("cardDataSeq");
					GachaResult.level = ResultCard->GetIntegerField("lv");
					GachaResult.id = ResultCard->GetIntegerField("id");

					if(gachaPackID == "ap001") //광고필드에만 존재하는 JSON값 예외 처리
					{
						TSharedPtr<FJsonObject> data = res->GetObjectField("data");
						GachaResult.number = data->GetIntegerField("number");
					}

				}
				Finished.Broadcast(resultCode, true, TEXT(""), GachaResult);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), GachaResult);
			}
		}
	}
	SetReadyToDestroy();
}

UGetCardPackList* UGetCardPackList::GetCardPackList()
{
	UGetCardPackList* BPNode = NewObject<UGetCardPackList>();
	return BPNode;
}

void UGetCardPackList::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/my-list"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("GET"));

	TArray<FCardPackList> ResultItems;
	ResultItems.Empty();

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetCardPackList::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("GetMyCardList "));
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UGetMyWatchItem - cannot process request"));
		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("cannot process request"), ResultItems);
		SetReadyToDestroy();
	}
}

void UGetCardPackList::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TArray<FCardPackList> ResultItems;

		ResultItems.Empty();
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ResultItems);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		TArray<FCardPackList> ResultItems;
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

		TArray<FCardPackList> ResultItems;
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
					auto ResultArray = res->GetObjectField("data")->GetArrayField("cardPackList");
					for (auto Elem : ResultArray)
					{
						const TSharedPtr<FJsonObject> Info = Elem->AsObject();
						FCardPackList ArrayElem;
						ArrayElem.infoSeq = Info->GetIntegerField("infoSeq");
						ArrayElem.gachaPackId = Info->GetStringField("gachaPackId");
						ArrayElem.name = Info->GetStringField("name");
						ArrayElem.type = Info->GetStringField("type");
						ArrayElem.coin = Info->GetIntegerField("coin");

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

UPostCardUpgradetutorial* UPostCardUpgradetutorial::PostCardUpgradetutorial(FString cardUID)
{
	UPostCardUpgradetutorial* BPNode = NewObject<UPostCardUpgradetutorial>();
	BPNode->cardUID = cardUID;

	return BPNode;
}

void UPostCardUpgradetutorial::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/upgrade"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("uid", cardUID);
	_payloadJson->SetStringField("tutorial","Y");
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostCardUpgradetutorial::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), -1);
		SetReadyToDestroy();
	}
}

void UPostCardUpgradetutorial::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), -1);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, -1);
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
				Finished.Broadcast(resultCode, true, TEXT(""), resultCode);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), resultCode);
			}
		}
	}
	SetReadyToDestroy();
}
UPostCardUpgrade* UPostCardUpgrade::PostCardUpgrade(FString cardUID)
{
	UPostCardUpgrade* BPNode = NewObject<UPostCardUpgrade>();
	BPNode->cardUID = cardUID;

	return BPNode;
}

void UPostCardUpgrade::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/upgrade"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("uid", cardUID);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostCardUpgrade::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), -1);
		SetReadyToDestroy();
	}
}

void UPostCardUpgrade::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), -1);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, -1);
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
				Finished.Broadcast(resultCode, true, TEXT(""), resultCode);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), resultCode);
			}
		}
	}
	SetReadyToDestroy();
}

UPostCardGameResult* UPostCardGameResult::PostCardGameResult(int32 enemyUserSeq, FString gameRoomName, int32 gameResult, int32 gamePoint)
{
	UPostCardGameResult* BPNode = NewObject<UPostCardGameResult>();
	BPNode->enemyUserSeq = enemyUserSeq;
	BPNode->gameRoomName = gameRoomName;
	BPNode->gameResult = gameResult;
	BPNode->gamePoint = gamePoint;

	return BPNode;
}

void UPostCardGameResult::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/game-result"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("enemyUserSeq", enemyUserSeq);
	_payloadJson->SetStringField("gameRoomName", gameRoomName);
	_payloadJson->SetNumberField("gameResult", gameResult);
	_payloadJson->SetNumberField("gamePoint", gamePoint);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostCardGameResult::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		FCardGameResult ResultReturn;
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), ResultReturn);
		SetReadyToDestroy();
	}
}

void UPostCardGameResult::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		FCardGameResult ResultReturn;
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ResultReturn);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		FCardGameResult ResultReturn;
		Finished.Broadcast(Response->GetResponseCode(), false, msg, ResultReturn);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		FCardGameResult ResultReturn;

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
				auto const returnObject = res->GetObjectField("data");
				ResultReturn.nowTrophy = FCString::Atoi(*returnObject->GetStringField("nowTrophy"));
				ResultReturn.preTrophy = FCString::Atoi(*returnObject->GetStringField("preTrophy"));
				ResultReturn.coin = returnObject->GetObjectField("user")->GetNumberField("coin");
				ResultReturn.crystal = returnObject->GetObjectField("user")->GetNumberField("crystal");
				ResultReturn.trophyBoostPoint = returnObject->GetNumberField("trophyBoostPoint");
				ResultReturn.trophyBoostPercent = FCString::Atof(*returnObject->GetStringField("trophyBoostPercent").TrimQuotes());

				Finished.Broadcast(resultCode, true, TEXT(""), ResultReturn);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), ResultReturn);
			}
		}
	}
	SetReadyToDestroy();
}

UPostCardGameRank* UPostCardGameRank::PostCardGameRank(int32 rankType)
{
	UPostCardGameRank* BPNode = NewObject<UPostCardGameRank>();
	BPNode->rankType = rankType;

	return BPNode;
}

void UPostCardGameRank::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/game-ranking"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("rankType", rankType);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostCardGameRank::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		TArray<FCardRankElem> ResultReturn;
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), ResultReturn,-1);
		SetReadyToDestroy();
	}
}

void UPostCardGameRank::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TArray<FCardRankElem> ResultReturn;
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), ResultReturn,-1);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		TArray<FCardRankElem> ResultReturn;
		Finished.Broadcast(Response->GetResponseCode(), false, msg, ResultReturn,-1);
		SetReadyToDestroy();
		return;
	}

	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TArray<FCardRankElem> RankArray;
		RankArray.Empty();

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
				auto const returnObject = res->GetObjectField("data");
				for (auto Elem : returnObject->GetArrayField("cardRankList"))
				{
					const auto Info = Elem->AsObject();
					FCardRankElem ArrayElem;
					ArrayElem.rank = Info->GetNumberField("rank");
					ArrayElem.trophy = Info->GetNumberField("trophy");
					ArrayElem.clubSeq = FCString::Atoi(*Info->GetStringField("clubSeq"));
					ArrayElem.countrySP = Info->GetStringField("countrySP");
					ArrayElem.nickName = Info->GetStringField("nickName");
					ArrayElem.userSeq = FCString::Atoi(*Info->GetStringField("userSeq"));

					RankArray.Add(ArrayElem);
				}
				int32 myTrophy = returnObject->GetNumberField("myTrophy");

				Finished.Broadcast(resultCode, true, TEXT(""), RankArray, myTrophy);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), RankArray,-1);
			}
		}
	}
	SetReadyToDestroy();
}

UPostDeductGold* UPostDeductGold::PostDeductGold(int32 GamePoint, FString GamePointType)
{
	UPostDeductGold* BPNode = NewObject<UPostDeductGold>();
	BPNode->GamePoint = GamePoint;
	BPNode->GamePointType = GamePointType;

	return BPNode;
}

void UPostDeductGold::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/game-play"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("gamePoint", GamePoint);
	_payloadJson->SetStringField("gamePointType", GamePointType);


	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostDeductGold::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void UPostDeductGold::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
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
UPostNFTCardUpgrade* UPostNFTCardUpgrade::PostNFTCardUpgrade(FString cardUID)
{
	UPostNFTCardUpgrade* BPNode = NewObject<UPostNFTCardUpgrade>();
	BPNode->cardUID = cardUID;

	return BPNode;
}

void UPostNFTCardUpgrade::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/nft-upgrade"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("uid", cardUID);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostNFTCardUpgrade::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), -1);
		SetReadyToDestroy();
	}
}

void UPostNFTCardUpgrade::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), -1);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, -1);
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
				Finished.Broadcast(resultCode, true, TEXT(""), resultCode);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), resultCode);
			}
		}
	}
	SetReadyToDestroy();
}

UPostNFTCardGradeUpgrade* UPostNFTCardGradeUpgrade::PostNFTCardGradeUpgrade(FString cardUID)
{
	UPostNFTCardGradeUpgrade* BPNode = NewObject<UPostNFTCardGradeUpgrade>();
	BPNode->cardUID = cardUID;

	return BPNode;
}

void UPostNFTCardGradeUpgrade::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/card/nft-teir-upgrade"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("uid", cardUID);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostNFTCardGradeUpgrade::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("ReportUser : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UReportUser - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"), -1);
		SetReadyToDestroy();
	}
}

void UPostNFTCardGradeUpgrade::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), -1);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg, -1);
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
				Finished.Broadcast(resultCode, true, TEXT(""), resultCode);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), resultCode);
			}
		}
	}
	SetReadyToDestroy();
}