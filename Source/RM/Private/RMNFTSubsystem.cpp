#include "RMNFTSubsystem.h"
#include "Async/Async.h"
#include "../RMProtocolFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "GenericPlatform/GenericPlatformHttp.h"


void FNFT::FromJson(const FString& JsonString) {
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,this);
}

void FNFT::FromObject(TSharedPtr<FJsonObject> JsonObject) {
	FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(),this);
	//if (OwnerUserName==)
	if (OwnerAddress == "0x0000000000000000000000000000000000000000") {
		OwnerUserName = "Join Now";
	}
	else {
		if (OwnerUserName=="") {
			OwnerUserName = "No Name";
		}
	}
//	if (TestString == "") {
		//printf("");
//	}
//	if (OwnerUserName == "NullAddress") {
//		OwnerUserName = "Join Now";
//	}
}

bool URMNFTSubsystem::SetNFTListData(const TSharedPtr<FJsonObject> Data)
{
	if(Data.Get() != nullptr)
	{
		const TArray<TSharedPtr<FJsonValue>> List = Data->GetArrayField(TEXT("nft"));
		for(TSharedPtr<FJsonValue> Val : List)
		{
			TSharedPtr<FJsonObject> obj = Val->AsObject();
			FNFT NFTInfo;
			FJsonObjectConverter::JsonObjectToUStruct(obj.ToSharedRef(), &NFTInfo);

			if(NFTListInfoTable.Contains(NFTInfo.Seq))
				NFTListInfoTable[NFTInfo.Seq] = NFTInfo;
			else
				NFTListInfoTable.Add(NFTInfo.Seq, NFTInfo);
		}
		return true;
	}
	return false;
}


void URMNFTSubsystem::RequestNFTList(FString thumbnail, const FOnHttpRequestResponseDelegate& InResponse, const FOnHttpRequestErrorDelegate& InError)
{
	OnRequestNftInfoFinished = InResponse;
	OnRequestNftInfoError = InError;

	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("/qatar/nft-search"), TEXT("POST"));
	TSharedPtr<FJsonObject> _PayloadJson = MakeShareable(new FJsonObject());
	_PayloadJson->SetStringField("thumbnail", thumbnail);
	FString _Payload;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_Payload);
	FJsonSerializer::Serialize(_PayloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_Payload);
	URMProtocolFunctionLibrary::ProcessDefaultHttp(HttpRequest, OnRequestNftInfoFinished, OnRequestNftInfoError, [&](TSharedPtr<FJsonObject> res)
	{
		const TSharedPtr<FJsonObject> Data = res->GetObjectField("data");

		if(SetNFTListData(Data))
			UE_LOG(LogRMProtocol, Warning, TEXT("Succeeded - %s"), *((FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))));

		FText OutVal;
		FJsonObjectConverter::GetTextFromObject(res.ToSharedRef(), OutVal);
		EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestResponseDelegate, OnRequestNftInfoFinished, OutVal.ToString());
		OnUpdateNftList.Broadcast();
	});
	
}


TSharedPtr<FJsonObject> URMNFTSubsystem::Parsing(FHttpResponsePtr pResponse, int32& OutResponeCode) {
	OutResponeCode = pResponse->GetResponseCode();
	if (OutResponeCode == 200) {
		FString Json = pResponse->GetContentAsString();
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Json);
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) {
			return JsonParsed;
		}
	}
	return nullptr;
}


void URMNFTSubsystem::GetNFT(const FNFTErrorDelegate& InError, const FNFTResponeDelegate& InRespone, FString thumbnail)
{
	OnError   = InError;
	OnRespone = InRespone;

	// FString API = URMProtocolFunctionLibrary::GetBaseUrl() + "/qatar/nft-search";
	// TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = (&FHttpModule::Get())->CreateRequest();
	// HttpRequest->SetVerb("POST");
	// HttpRequest->SetURL(API);

	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("qatar/nft-search"), TEXT("POST"));

	TSharedPtr<FJsonObject> _PayloadJson = MakeShareable(new FJsonObject());
	_PayloadJson->SetStringField("thumbnail", thumbnail);
	FString _Payload;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_Payload);
	FJsonSerializer::Serialize(_PayloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_Payload);
	
	HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully)
	{
		if (connectedSuccessfully)
		{
			int32 ResponeCode = 0;
			TSharedPtr<FJsonObject> JsonParsed = Parsing(pResponse, ResponeCode);
			if (JsonParsed)
			{
				FString ResultCode = JsonParsed->GetStringField("resultCode");
				FString SessionId  = JsonParsed->GetStringField("sessionId");
				TSharedPtr<FJsonObject> Data = JsonParsed->GetObjectField("data");
				if (Data)
				{
					NFT.Empty();
					TArray<TSharedPtr<FJsonValue>> JsonNFT = Data->GetArrayField("nft");
					NFT.SetNum(JsonNFT.Num());
					for (int i = 0; i < JsonNFT.Num(); i++)
					{
						NFT[i].FromObject(JsonNFT[i]->AsObject());
					}
					OnRespone.ExecuteIfBound(pResponse->GetContentAsString());
				}
				else {
					//todo
				}
			}
			else {
				OnError.ExecuteIfBound(ResponeCode);
			}
		}
		else {
			OnError.ExecuteIfBound(443);
		}
		});


	HttpRequest->ProcessRequest();
}

FString URMNFTSubsystem::GetNFTURL()
{
	return URMProtocolFunctionLibrary::GetBaseUrl() + "/qatar/nft-search";
}

