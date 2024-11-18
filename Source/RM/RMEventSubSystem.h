// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RMStruct.h"
#include "HttpModule.h"
#include "Http.h"
#include "RMEventSubSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateEventDelegate, int32, EventSeq);


/**
 * Legacy EventSystem. (Attendance Event)
 */
UCLASS()
class RM_API URMEventSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	TMap<int, FEventInfo> GetEventInfo(); 	/* 모든 이벤트 정보를 불러옵니다. */


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	UPARAM(DisplayName = "IsValidate") bool GetEventInfoByEventSequence(int eventSeq, FEventInfo& returnInfo); 	/* 이벤트 시퀀스(서버값)을 Key값으로 Info를 불러옵니다. */


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	TArray<int> GetAllEventSequence(); 	/* 모든 이벤트 시퀀스(Key값)을 TArray에 담아 불러옵니다. */

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	TArray<FEventInfo> GetEventInfoAsArray(); /* 모든 이벤트 정보를 TArray에 담아 불러옵니다. */
 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	UPARAM(DisplayName = "AnyNew?") bool GetNewEventFlag(int& OpenDefualtEventSeq); /* 새로운 New가 있는지 알아옵니다. 만약 있다면 Default 이벤트 시퀀스를 올바르게 리턴합니다. */

	/* 
		새로운 New가 있는지 알아옵니다. 기본적으로 eventSeq를 기준으로 소팅됩니다.
		@Param bDescending: true이면 내림차순으로 계산합니다.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	UPARAM(DisplayName = "AnyNew?") bool GetNewEventFlagWithSort(int& OpenDefaultEventSeq, bool bDescending = false);

	/*
		새로운 New가 있는지 알아옵니다. 기본적으로 서버에서 준 우선순위로 시퀀스를 return 합니다.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	UPARAM(DisplayName = "AnyNew?") bool GetNewEventFlagWithPriority(int& OpenDefaultEventSeq);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMEventSubSystem")
	FString GetUserAccessToken();
	
	void SetEventInfo(TMap<int, FEventInfo> NewEventInfo);
	bool AddEventInfo(int eventSeq, FEventInfo newEvent);
	void ModifyEventInfo(FEventInfo newInfo);

	bool SetIsNewFlagForce_Internal(int eventSeq, bool value);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RMEventSubSystem")
	TMap<int, FEventInfo> EventInfo; /* EventInfo[Index] :: Index == EventInfo[index].eventSeq */

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "RMEventSubSystem")
	FUpdateEventDelegate OnUpdateEvent;
};

/* FOR Nested TMap Container */
UCLASS(Blueprintable, BlueprintType)
class UEventInfoListArray : public UObject
{
	GENERATED_BODY()
public:
	FEventInfoList& operator[] (int i) { return infoLists[i]; }

	UFUNCTION(BlueprintCallable)
	void Add(FEventInfoList item)
	{
		infoLists.Add(item);
	}

	UFUNCTION(BlueprintCallable)
	void Append(TArray<FEventInfoList> Items)
	{
		infoLists.Append(Items.GetData(), Items.Num());
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPARAM(DisplayName = "Success?") bool Get(int index, FEventInfoList& item)
	{
		if (infoLists.IsValidIndex(index))
		{
			item = infoLists[index];
			return true;
		}
		return false;
	}

	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "Success?") bool RemoveItemByIndex(int index)
	{
		if (infoLists.IsValidIndex(index))
		{
			infoLists.RemoveAt(index);
			return true;
		}
		return false;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 Length()
	{
		return infoLists.Num();
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEventInfoList> infoLists;

};

USTRUCT(BlueprintType, Blueprintable)
struct FPageInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 totalPage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 currentPage;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnRequestEventInfoListComplete, int32, ResultCode, FString, ResultMessage, bool, bIsSucceeded);

UCLASS()
class RM_API URMWebEventSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	/* 현재 가지고 있는 각 언어별로 모든 이벤트 정보를 불러옵니다. */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UEventInfoListArray* GetEventListInfos(FString LocaleCode)
	{
		if (eventListInfos.Find(LocaleCode))
			return eventListInfos[LocaleCode];
		else
			return nullptr;
	}

	UFUNCTION(BlueprintCallable)
	void ResetEventListInfos();
	
	/* 각 언어별 이벤트 Info에 addList를 붙입니다. */
	UFUNCTION(BlueprintCallable)
	void AppendEventListInfo(FString LocaleCode, TArray<FEventInfoList> addedList);

	/* 각 언어별 이벤트의 다음 페이지를 로드할 수 있는 상태인지 알아냅니다. */ 
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CheckCanLoadNextPage(FString LocaleCode);

	/* 각 LocaleCode에 맞는 이벤트 리스트의 다음 페이지를 요청합니다. */
	UFUNCTION(BlueprintCallable)
	void RequestEventInfoList(FString LocaleCode, int32 page, int32 showCount, const FOnRequestEventInfoListComplete& OnRequestComplete);
	
protected:
	/* Key = LocaleCode, Value = EventInfos in LocaleCode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewRMEventSubSystem")
	TMap<FString, UEventInfoListArray*> eventListInfos;
	/* Key = LocaleCode, Value = EventPageInfos in LocaleCode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewRMEventSubSystem")
	TMap<FString, FPageInfo> eventListPageInfos;


private:
	/* HTTP REQUEST */
	void ActivateHttpRequest(int32 page, int32 showCnt, FString LocaleCode);
	void OnResponseHttpRequest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	/* HTTP REQUEST FIN */
	UFUNCTION()
	void TriggerFinishedRequestEvent(int32 resultCode, FString errorMessage, bool isSucceeded);
private:
	/* Saved showCountPerOnce */
	UPROPERTY()
	int32 showCountPerOnce;

	/* Finished Delegate Request Http */
	UPROPERTY()
	FOnRequestEventInfoListComplete OnRequestEventInfoListCompleted;

	/* Current Locale Code */
	static FString CurrentLocale; 
};
