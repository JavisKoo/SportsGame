// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestSubsystem.h"
#include "Quest/QuestSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Http.h"
#include "RM/RMProtocolFunctionLibrary.h"
#include "JsonObjectConverter.h"

DEFINE_LOG_CATEGORY(LogQuest);


UQuestSubsystem::UQuestSubsystem()
{
	// 1. 
	static ConstructorHelpers::FObjectFinder<UDataTable> DailyTableObject(TEXT("/Game/RM/Quest/DailyTable"));
	if (DailyTableObject.Succeeded())
	{
		DailyDataTable = DailyTableObject.Object;
	}
}

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogQuest, Log, TEXT("UQuestSubsystem::Initialize"));
}

// TIP: ������ ȣ�� ������ GetUserInfo �� �̴�.
void UQuestSubsystem::Init()
{
	Request_DailyQuests();
}

void FAttendanceInfo::FromObject(TSharedPtr<FJsonObject> JsonObject)
{
	FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), this);
}


#pragma region Mission
void UQuestSubsystem::DoEmoticon(EMapType Place, FName RowName)
{
	CheckMission(EMissionType::Emoticon, Place, RowName.ToString());
}

void UQuestSubsystem::DoGesture(EMapType Place, FName RowName)
{
	CheckMission(EMissionType::Gesture, Place, RowName.ToString());
}

void UQuestSubsystem::DoContentCheck(FName RowName)
{
	for (auto& mission : Daily)
	{
		if (mission.MissionType == EMissionType::ContentCheck &&
			mission.State == EQuestState::None &&
			mission.MissionValue == RowName.ToString())
		{
			mission.State = EQuestState::Successful;

			OnChangedQuestDelegate.Broadcast(HasSuccessfulQuest(), mission.State);

			SaveGame();
			break;
		}
	}
}

void UQuestSubsystem::DoChat(EMapType Place)
{
	CheckMission(EMissionType::Chat, Place);
}

void UQuestSubsystem::DoVoiceChat(EMapType Place)
{
	CheckMission(EMissionType::VoiceChat, Place);
}

void UQuestSubsystem::CreateRoom(EMapType Place)
{
	CheckMission(EMissionType::CreateRoom, Place);
}

void UQuestSubsystem::JoinUserRoom(EMapType Place)
{
	CheckMission(EMissionType::JoinUserRoom, Place);
}

void UQuestSubsystem::JoinDefaultRoom(EMapType Place, FString RoomName)
{
	CheckMission(EMissionType::JoinDefaultRoom, Place, RoomName);
}

EQuestState UQuestSubsystem::SolveQuiz(FName QuestID, bool IsSuccessful)
{
	for (auto& mission : Daily)
	{
		if (mission.MissionType == EMissionType::Quiz &&
			mission.State == EQuestState::None)
		{
			mission.State = IsSuccessful ? EQuestState::Successful : EQuestState::Failed;

			if (IsSuccessful)
				OnChangedQuestDelegate.Broadcast(HasSuccessfulQuest(), mission.State);

			SaveGame();
			return mission.State;
		}
	}

	return IsSuccessful ? EQuestState::Successful : EQuestState::Failed;
}

#pragma endregion



// ����Ʈ�� �����ϴ�.
void UQuestSubsystem::DecideeQuest(TArray<FName>& ServerList)
{
	// 1. Load SaveGame
	UQuestSaveGame* LoadGameInstance = Cast<UQuestSaveGame>(UGameplayStatics::CreateSaveGameObject(UQuestSaveGame::StaticClass()));
	LoadGameInstance = Cast<UQuestSaveGame>(UGameplayStatics::LoadGameFromSlot(*QuestSaveSlotName, URMProtocolFunctionLibrary::GetUserSeq()));

	if (LoadGameInstance == nullptr)
	{
		UE_LOG(LogQuest, Log, TEXT("LoadGameInstance is nullptr"));
	}
	else
	{
		Daily = LoadGameInstance->Daily;
	}

	/*ServerList.Empty();
	ServerList.Add(FName(TEXT("Daily_0003")));
	ServerList.Add(FName(TEXT("Daily_0080")));
	ServerList.Add(FName(TEXT("Quiz_0012")));*/


	// 1. ����Ʈ ������ üũ�Ѵ�.
	if (ServerList.Num() != 3)
	{
		UE_LOG(LogQuest, Error, TEXT("There are not three Daily Quest"));
		return;
	}

	// 2. ���ο� ����Ʈ�� ���� �����Ͱ� ������ �˻��Ѵ�.
	bool IsSame = true;
	if (Daily.Num() == 0)	// ���̺� �����Ͱ� �������� �ʴ´�.
	{
		IsSame = false;
	}	
	else
	{
		for (int32 i = 0; i != ServerList.Num(); ++i)
		{
			if (ServerList[i] != Daily[i].QuestID)
			{
				IsSame = false;
				break;
			}
		}
	}
	
	// 3. 
	if (!IsSame)	// ���̺� �����Ϳ� �ٸ� ������
	{
		Daily.Empty(3);
		for (int32 i = 0; i != ServerList.Num(); ++i)
		{
			FQuestState q;
			q.QuestID = ServerList[i];

			FDailyQuestTable* Row = DailyDataTable->FindRow<FDailyQuestTable>(q.QuestID, FString(""));
			if (Row == nullptr)
			{
				UE_LOG(LogQuest, Error, TEXT("Not found Daily Quest Table : %s"), *q.QuestID.ToString());
			}
			else
			{
				q.MissionType = Row->MissionType;
				q.MissionPlace = Row->MissionPlace;
				q.MissionValue = Row->MissionValue;

				UE_LOG(LogQuest, Log, TEXT("Daily Quest Table : %s"), *q.QuestID.ToString());
			}
			Daily.Add(q);
		}
	}
}

void UQuestSubsystem::SaveGame()
{
	UQuestSaveGame* SaveGameInstance = Cast<UQuestSaveGame>(UGameplayStatics::CreateSaveGameObject(UQuestSaveGame::StaticClass()));
	SaveGameInstance->Daily = Daily;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, *QuestSaveSlotName, URMProtocolFunctionLibrary::GetUserSeq());
	
} 

void UQuestSubsystem::CheckMission(EMissionType MissionType, EMapType Place)
{
	UE_LOG(LogQuest, Log, TEXT("CheckMission : %d, %d"), (int)MissionType, (int)Place);
	for (auto& mission : Daily)
	{
		UE_LOG(LogQuest, Log, TEXT("mi : %d, %d"), (int)mission.MissionType, (int)mission.MissionPlace);
		if (mission.MissionType == MissionType &&
			mission.State == EQuestState::None &&
			(mission.MissionPlace == EMapType::None || mission.MissionPlace == Place))
		{
			UE_LOG(LogQuest, Log, TEXT("Successful"));

			mission.State = EQuestState::Successful;

			OnChangedQuestDelegate.Broadcast(HasSuccessfulQuest(), mission.State);

			SaveGame();
			break;
		}
	}
}

void UQuestSubsystem::CheckMission(EMissionType MissionType, EMapType Place, FString Value)
{
	UE_LOG(LogQuest, Log, TEXT("CheckMission : %d, %d, %s"), (int)MissionType, (int)Place, *Value);
	for (auto& mission : Daily)
	{
		UE_LOG(LogQuest, Log, TEXT("mi : %d, %d, %s"), (int)mission.MissionType, (int)mission.MissionPlace, *mission.MissionValue);
		if (mission.MissionType == MissionType &&
			mission.State == EQuestState::None &&
			mission.MissionValue == Value &&
			(mission.MissionPlace == EMapType::None || mission.MissionPlace == Place))
		{
			UE_LOG(LogQuest, Log, TEXT("Successful"));
			mission.State = EQuestState::Successful;

			OnChangedQuestDelegate.Broadcast(HasSuccessfulQuest(), mission.State);

			SaveGame();
			break;
		}
	}
}



TArray<FQuestState> UQuestSubsystem::GetDailyQuestList() const
{
	return Daily;
}

bool UQuestSubsystem::HasSuccessfulQuest()
{
	/*if (SaveGame->Daily.Num() == 0)
		return false;*/

	for (auto& mission : Daily)
	{
		if (mission.State == EQuestState::Successful)
			return true;
	}

	return false;
}

bool UQuestSubsystem::CompleteQuest(FName QuestID)
{
	UE_LOG(LogQuest, Log, TEXT("CompleteQuest : %s"), *QuestID.ToString());

	for (auto& mission : Daily)
	{
		UE_LOG(LogQuest, Log, TEXT("mi : %s"), *mission.QuestID.ToString());

		if (mission.QuestID == QuestID && mission.State == EQuestState::Successful)
		{
			UE_LOG(LogQuest, Log, TEXT("Completed : %s"), *QuestID.ToString());

			mission.State = EQuestState::Completed;

			OnChangedQuestDelegate.Broadcast(HasSuccessfulQuest(), mission.State);

			SaveGame();

			// Web Server
			Request_GetQuestReward(mission.QuestID);

			return true;
		}
	}

	return false;
}


#pragma region Web Protocol

void UQuestSubsystem::Request_DailyQuests()
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("quest/today"), TEXT("GET"));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UQuestSubsystem::Response_DailyQuests);
		UE_LOG(LogQuest, Log, TEXT("RequestDailyQuests : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogQuest, Warning, TEXT("RequestDailyQuests - cannot process request"));
	}
}

void UQuestSubsystem::Response_DailyQuests(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (Response == nullptr || !WasSuccessful || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogQuest, Warning, TEXT("RequestDailyQuests Fail!!"));
		return;
	}

	UE_LOG(LogQuest, Warning, TEXT("RequestDailyQuests : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			TArray<FName> Result;

			TSharedPtr<FJsonObject> data = res->GetObjectField(TEXT("data"));
			const TArray<TSharedPtr<FJsonValue>> todayList = data->GetArrayField("todayList");
			for (TSharedPtr<FJsonValue> j : todayList)
			{
				TSharedPtr<FJsonObject> daily = j->AsObject();
				Result.Add(FName(*daily->GetStringField("id")));
			}

			const TArray<TSharedPtr<FJsonValue>> quizList = data->GetArrayField("quiz");
			for (TSharedPtr<FJsonValue> j : quizList)
			{
				TSharedPtr<FJsonObject> quiz = j->AsObject();
				Result.Add(FName(*quiz->GetStringField("id")));
			}

			DecideeQuest(Result);
		}
	}
}

void UQuestSubsystem::Request_GetQuestReward(FName QuestID)
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("quest/reward"), TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("rewardSeq", QuestID.ToString());
	_payloadJson->SetStringField("rewardType", FString::FromInt(2));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UQuestSubsystem::Response_GetQuestReward);
		UE_LOG(LogQuest, Log, TEXT("Request_GetQuestReward : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogQuest, Warning, TEXT("Request_GetQuestReward - cannot process request"));
	}
}

void UQuestSubsystem::Response_GetQuestReward(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (Response == nullptr || !WasSuccessful || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogQuest, Warning, TEXT("Response_GetQuestReward Fail!!"));
		return;
	}

	UE_LOG(LogQuest, Warning, TEXT("Response_GetQuestReward : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
		}
		else
		{
			UE_LOG(LogQuest, Error, TEXT("%s"), *res->GetStringField("resultMessage"));
		}
	}
}

void UQuestSubsystem::Request_GetTicketReward(const FAttendanceErrorDelegate& ErrorDelegate, const FAttendanceResponseDelegate& SuccessDelegate)
{
	OnError =ErrorDelegate;
	OnResponse = SuccessDelegate;
	
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("quest/attendance/reward"), TEXT("GET"));

	HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully)
	{
		if (connectedSuccessfully)
		{
			if (pResponse==nullptr)
			{
				OnError.ExecuteIfBound(0);
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
					AttendanceInfo.FromObject(Data);
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
#pragma endregion
