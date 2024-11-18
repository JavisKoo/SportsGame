// Fill out your copyright notice in the Description page of Project Settings.
#include "RMEventSubSystem.h"

#include <string>
#include "RMProtocolFunctionLibrary.h"
#include "RMStruct.h"
#include "RM_Singleton.h"
#include "RMProtocol.h"

/*
 * Legacy RM Event Source Impl
 */
#pragma region LEGACY_RMEVENT
TMap<int, FEventInfo> URMEventSubSystem::GetEventInfo()
{
	if (EventInfo.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("URMEventSubSystem::GetEventInfo() - EventInfo == NULL Check it Again."));
	}
	return EventInfo;
}

bool URMEventSubSystem::GetEventInfoByEventSequence(int eventSeq, FEventInfo& returnInfo)
{
	returnInfo = FEventInfo();
	bool validate = EventInfo.Contains(eventSeq);
	if (validate)
		returnInfo = EventInfo[eventSeq];
	
	return validate;
}

TArray<int> URMEventSubSystem::GetAllEventSequence()
{
	TArray<int> AllKeys;
	EventInfo.GetKeys(AllKeys);

	return AllKeys;
}

TArray<FEventInfo> URMEventSubSystem::GetEventInfoAsArray()
{
	TArray<FEventInfo> infoArr;
	EventInfo.GenerateValueArray(infoArr);
	return infoArr;
}

bool URMEventSubSystem::GetNewEventFlag(int& OpenDefualtEventSeq)
{
	OpenDefualtEventSeq = 0;
	bool bHasAnyNewFlag = false;
	for (FEventInfo info : GetEventInfoAsArray())
	{
		/* 예외 처리 == 출첵 이벤트 */
		if (info.eventSeq == 1) continue;

		if (info.bIsNew)
		{
			OpenDefualtEventSeq = info.eventSeq;
			bHasAnyNewFlag = true;
			break;
		}
	}
	return bHasAnyNewFlag;
}

bool URMEventSubSystem::GetNewEventFlagWithSort(int& OpenDefaultEventSeq, bool bDescending)
{
	if (!bDescending)
		return GetNewEventFlag(OpenDefaultEventSeq);
	
	OpenDefaultEventSeq = 0;
	bool bHasAnyNewFlag = false;
	TArray<int> eventSeqArr = GetAllEventSequence();
	TArray<FEventInfo> infoArr = GetEventInfoAsArray();
	infoArr.Sort([](const FEventInfo& A, const FEventInfo& B) {
		// Compare Lambda
		return A.eventSeq > B.eventSeq;
		}
	);

	for (FEventInfo info : infoArr)
	{
		if (info.eventSeq == 1) continue;

		if (info.bIsNew)
		{
			OpenDefaultEventSeq = info.eventSeq;
			bHasAnyNewFlag = true;
			break;
		}
	}
	return bHasAnyNewFlag;
}

bool URMEventSubSystem::GetNewEventFlagWithPriority(int& OpenDefaultEventSeq)
{
	OpenDefaultEventSeq = 0;
	bool bHasAnyNewFlag = false;
	TArray<int> eventSeqArr = GetAllEventSequence();
	TArray<FEventInfo> infoArr = GetEventInfoAsArray();
	infoArr.Sort([](const FEventInfo& A, const FEventInfo& B) {
		return A.priority < B.priority;
		}
	);
	for (FEventInfo info : infoArr)
	{
		if (info.eventSeq == 1) continue;

		if (info.bIsNew)
		{
			OpenDefaultEventSeq = info.eventSeq;
			bHasAnyNewFlag = true;
			break;
		}
	}
	return bHasAnyNewFlag;
}

FString URMEventSubSystem::GetUserAccessToken()
{
	//get accesstoken
	FString accessToken = URMProtocolFunctionLibrary::GetAccessToken();
	return accessToken;
}

void URMEventSubSystem::SetEventInfo(TMap<int, FEventInfo> NewEventInfo)
{
	EventInfo.Empty(0);
	EventInfo = NewEventInfo;
}

bool URMEventSubSystem::AddEventInfo(int eventSeq, FEventInfo newEvent)
{
	if (EventInfo.Contains(eventSeq))
		return false;
	EventInfo.Add(eventSeq, newEvent);
	return true;
}

void URMEventSubSystem::ModifyEventInfo(FEventInfo newInfo)
{
	if (EventInfo.Contains(newInfo.eventSeq))
	{
		int priority = EventInfo[newInfo.eventSeq].priority;
		newInfo.priority = priority;
		EventInfo[newInfo.eventSeq] = newInfo;
	}
}

bool URMEventSubSystem::SetIsNewFlagForce_Internal(int eventSeq, bool value)
{
	bool isValidate = EventInfo.Contains(eventSeq);
	if (isValidate)
		EventInfo[eventSeq].bIsNew = value;
	return isValidate;
}
#pragma endregion 

/*
 * New WebView RM Event Source Impl
 */
#pragma region WEBVIEW_RMEVENT

FString URMWebEventSubSystem::CurrentLocale = FString::Printf(TEXT("EN"));

void URMWebEventSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentLocale = TEXT("EN");
	showCountPerOnce = 20;
}

void URMWebEventSubSystem::ResetEventListInfos()
{
	eventListInfos.Empty();
	eventListPageInfos.Empty();
}

void URMWebEventSubSystem::AppendEventListInfo(FString LocaleCode, TArray<FEventInfoList> addedList)
{
	if (!eventListInfos.Find(LocaleCode))
	{
		auto eventInfoListArr = NewObject<UEventInfoListArray>();
		eventListInfos.Add(LocaleCode, eventInfoListArr);
	}
		
	eventListInfos[LocaleCode]->Append(addedList);
}

bool URMWebEventSubSystem::CheckCanLoadNextPage(FString LocaleCode)
{
	if (!eventListPageInfos.Find(LocaleCode))
	{
		FPageInfo page;
		page.totalPage = 1;
		page.currentPage = 0;
		eventListPageInfos.Add(LocaleCode, page);
	}
	
	if (eventListPageInfos[LocaleCode].totalPage > eventListPageInfos[LocaleCode].currentPage)
		return true;
	
	return false;
}

void URMWebEventSubSystem::RequestEventInfoList(FString LocaleCode, int32 page, int32 showCount,
	const FOnRequestEventInfoListComplete& OnRequestComplete)
{
	if (OnRequestEventInfoListCompleted.IsBound())
		OnRequestEventInfoListCompleted.Clear();

	OnRequestEventInfoListCompleted = OnRequestComplete;
	CurrentLocale = LocaleCode;
	ActivateHttpRequest(page, showCount, LocaleCode);
	// UGetEventListInfo* AsyncNode = UGetEventListInfo::GetEventListInfo(page, showCount, LocaleCode);
	// AsyncNode->Finished.AddDynamic(this, &UNewRMEventSubSystem::FinishRequestEventInfoList);
}

void URMWebEventSubSystem::ActivateHttpRequest(int32 page, int32 showCnt, FString LocaleCode)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/info/event"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("page", FString::FromInt(page));
	_payloadJson->SetStringField("showCnt", FString::FromInt(showCnt));
	_payloadJson->SetStringField("sortType", LocaleCode.IsEmpty() ? TEXT("EN") : LocaleCode);
	
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URMWebEventSubSystem::OnResponseHttpRequest);
		UE_LOG(LogRMProtocol, Log, TEXT("URequsetWebEventPage : %s"), *HttpRequest->GetURL());
	}
	else
	{
		TArray<FEventInfoList> Infos;
		UE_LOG(LogRMProtocol, Warning, TEXT("URequsetWebEventPage - cannot process request"));
		TriggerFinishedRequestEvent(0, TEXT("URequsetWebEventPage - cannot process request"), false);
	}
}

void URMWebEventSubSystem::OnResponseHttpRequest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	TArray<FEventInfoList> eventInfoLists;
	int totalPage = 0;
	int currentPage = 0;
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		TriggerFinishedRequestEvent(0, TEXT("The Internet connection appears to be offline."), false);
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		TriggerFinishedRequestEvent(Response->GetResponseCode(), msg, false);
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
				TriggerFinishedRequestEvent(Response->GetResponseCode(), TEXT("SomeThing Wrong"), false);
				return;
			}

			int resultCode = res->GetIntegerField("resultCode");
			FString resultMsg = res->GetStringField("resultMessage");
			if (res->HasField(TEXT("data")))
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
				FPageInfo page;
				page.totalPage = data->GetIntegerField("totalPage");
				page.currentPage = data->GetIntegerField("currentPage");
				eventListPageInfos.Add(CurrentLocale, page);
				
				TArray<TSharedPtr<FJsonValue>> listArr = data->GetArrayField("list");
				for (TSharedPtr<FJsonValue> item : listArr)
				{
					TSharedPtr<FJsonObject> obj = item->AsObject();
					if (obj)
					{
						FEventInfoList info;
						info.EventCode = (obj->HasField("eventCode")) ? obj->GetStringField("eventCode") : TEXT("NONE");
						info.eventSeq = (obj->HasField("eventSeq")) ? obj->GetIntegerField("eventSeq") : -1;
						info.Title = obj->HasField("title") ? obj->GetStringField("title") : TEXT("");
						info.URL = obj->HasField("url") ? obj->GetStringField("url") : TEXT("");
						if (obj->HasField("attendYN"))
							info.bCanAttend = (obj->GetStringField("attendYN") == TEXT("Y")) ? true : false;
						if (obj->HasField("isNew"))
							info.bIsNew = (obj->GetStringField("isNew") == TEXT("Y")) ? true : false;
						
						eventInfoLists.Add(info);
					}
				}
			}

			if (resultCode == 200)
			{
				AppendEventListInfo(CurrentLocale, eventInfoLists);
				TriggerFinishedRequestEvent(resultCode, TEXT("API REQUEST SUCCEESS"), true);
			}
			else
			{
				TriggerFinishedRequestEvent(resultCode, TEXT("API REQUEST FAILED"), false);
			}
		}
	}
}

void URMWebEventSubSystem::TriggerFinishedRequestEvent(int32 resultCode, FString errorMessage, bool isSucceeded)
{
	if (OnRequestEventInfoListCompleted.IsBound())
	{
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateLambda([resultCode, errorMessage, isSucceeded, this]()
			{
				OnRequestEventInfoListCompleted.ExecuteIfBound(resultCode, errorMessage, isSucceeded);
				OnRequestEventInfoListCompleted.Clear();
			}),
			TStatId(), nullptr, ENamedThreads::GameThread);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRequestEventInfoListCompleted Delegate is not Valid."));
		return;
	}
}
#pragma endregion 
