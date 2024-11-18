// Fill out your copyright notice in the Description page of Project Settings.


#include "AnalyticsFunctionLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "RM/RMBlueprintFunctionLibrary.h"
#include "RM/RMProtocolFunctionLibrary.h"

#define SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(ParamName, ...)								\
{																							\
	if (UAnalyticsFunctionLibrary::CanSendMessage())										\
	{																						\
		TArray<FAnalyticsParam> SendingArray( {__VA_ARGS__} );								\
		SendingArray.Append( CreateDefaultAnalyticsParameterArray() );						\
		UFirebaseFunctionLibrary::LogCustomEventWithParameterList(ParamName, SendingArray);	\
	}																						\
}	

bool UAnalyticsFunctionLibrary::CanSendMessage()
{
	//return URMBlueprintFunctionLibrary::GetCurrentBundleType() == EBundleType::Default;
	return true;
}

TArray<FAnalyticsParam> UAnalyticsFunctionLibrary::AddAnalyticsParamArray(TArray<FAnalyticsParam> ParamArray, FAnalyticsParam AddedParam)
{
	TArray<FAnalyticsParam> OutArray;
	OutArray.Add(AddedParam);
	return OutArray;
}

TArray<FAnalyticsParam> UAnalyticsFunctionLibrary::AppendAnalyticsParamArray(TArray<FAnalyticsParam> ParamArray, TArray<FAnalyticsParam> Appended)
{
	ParamArray.Append(Appended);
	return ParamArray;
}


TArray<FAnalyticsParam> UAnalyticsFunctionLibrary::CreateDefaultAnalyticsParameterArray()
{
	TArray<FAnalyticsParam> OutArray;
	const FString UserID = FString::FromInt(URMProtocolFunctionLibrary::GetUserSeq());
	//FAnalyticsParam Param = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("userId"), UserID);
	//OutArray.Add(Param);
	return OutArray;
}

TArray<FAnalyticsParam> UAnalyticsFunctionLibrary::CreateDefaultAnalyticsClickEventParameterArray()
{
	TArray<FAnalyticsParam> OutArray = CreateDefaultAnalyticsParameterArray();
	//UFirebaseFunctionLibrary::CreateInt64AnalyticsParameter(TEXT("click"), 1);
	return OutArray;
}

FAnalyticsParam UAnalyticsFunctionLibrary::CreateSpentTimeAnalyticsParameter(FDateTime EnterTime)
{
	const static float MaxTimeHour = 24.0f;
	const FString ParamValue = ((FDateTime::UtcNow() - EnterTime).GetTotalHours() <= MaxTimeHour)
	? UKismetTextLibrary::AsTimespan_Timespan(FDateTime::UtcNow() - EnterTime).ToString()
	: TEXT("");

	//FAnalyticsParam OutParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("spent_time"), ParamValue);
	//return OutParam;
	return FAnalyticsParam();
}

FAnalyticsParam UAnalyticsFunctionLibrary::CreateClickAnalyticsParameter(int64 Click)
{
	//FAnalyticsParam OutParam = UFirebaseFunctionLibrary::CreateInt64AnalyticsParameter(TEXT("click"), Click);
	//return OutParam;
	return FAnalyticsParam();
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_Sidebar(bool Click, FString Menu)
{
	const FAnalyticsParam ClickParam = CreateClickAnalyticsParameter(static_cast<int64>(Click));
	//const FAnalyticsParam MenuParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("menu"), Menu);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("sidebar"), ClickParam, MenuParam);	
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_UserListButton(bool Click, FString FollowStatus, FDateTime EnterTime)
{
	const FAnalyticsParam ClickParam = CreateClickAnalyticsParameter(static_cast<int64>(Click));
	//const FAnalyticsParam ToggleParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("toggle"), FollowStatus);
	const FAnalyticsParam SpentTimeParam = CreateSpentTimeAnalyticsParameter(EnterTime);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("btn_user_list"), ClickParam, ToggleParam, SpentTimeParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_EmogiButton(FString EmogiID)
{
	//const FAnalyticsParam ImogiIDParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("emogi_id"), EmogiID);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("talk_emogi"), ImogiIDParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_Test_Button(FString TestID)
{
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_GestureButton(FString GestureID)
{
	//const FAnalyticsParam GestureIDParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("gesture_id"), GestureID);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("talk_gesture"), GestureIDParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_Level(FString LevelName, FString RoomType, FDateTime EnterTime)
{
	//const FAnalyticsParam LevelNameParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("name"), LevelName);
	//const FAnalyticsParam RoomTypeParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("room_type"), RoomType);
	const FAnalyticsParam SpentTimeParam = CreateSpentTimeAnalyticsParameter(EnterTime);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("map"), LevelNameParam, RoomTypeParam, SpentTimeParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_MarketButton(bool Click)
{
	const FAnalyticsParam ClickParam = CreateClickAnalyticsParameter(static_cast<int64>(Click));
	SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("market_show"), ClickParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_Costume(bool Click, bool Buy, FString Type, FString ItemID, FDateTime EnterTime)
{
	const FAnalyticsParam ClickParam = CreateClickAnalyticsParameter(static_cast<int64>(Click));
	//const FAnalyticsParam BuyParam = UFirebaseFunctionLibrary::CreateInt64AnalyticsParameter(TEXT("buy"), static_cast<int64>(Buy));
	//const FAnalyticsParam TypeParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("type"), Type);
	//const FAnalyticsParam ItemIDParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("itemID"), ItemID);
	const FAnalyticsParam SpentTimeParam = CreateSpentTimeAnalyticsParameter(EnterTime);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("shop_costume"), ClickParam, BuyParam, TypeParam, ItemIDParam, SpentTimeParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_BoardEvent(bool Click, FString Type, FString EventCode)
{
	const FAnalyticsParam ClickParam = CreateClickAnalyticsParameter(static_cast<int64>(Click));
	//const FAnalyticsParam TypeParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("type"), Type);
	//const FAnalyticsParam EventCodeParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("eventCode"), EventCode);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("board_event"), ClickParam, TypeParam, EventCodeParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_MiniGame(FString Type, FDateTime EnterTime)
{
	//const FAnalyticsParam TypeParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("type"), Type);
	const FAnalyticsParam SpentTimeParam = CreateSpentTimeAnalyticsParameter(EnterTime);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("minigame"), TypeParam, SpentTimeParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_CreateRoom(bool bIsCreate, FString Type, FString LevelName)
{
	//const FAnalyticsParam IsCreateParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("action"),
	//	(bIsCreate) ? TEXT("create") : TEXT("cancel"));
	//const FAnalyticsParam TypeParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("type"), Type);
	//const FAnalyticsParam LevelNameParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("map_name"), LevelName);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("btn_create_room"), IsCreateParam, TypeParam, LevelNameParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_CancelAdditionalDownloads(bool bIsCanceled)
{
	//const FAnalyticsParam CancelParam = UFirebaseFunctionLibrary::CreateInt64AnalyticsParameter(TEXT("cancel"), static_cast<int64>(bIsCanceled));
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("additional_downloads"), CancelParam);
}

void UAnalyticsFunctionLibrary::SendAnalyticsEvent_Error(FString VoiceErrorValue, FString PhotonReconnectedErrorValue)
{
	//const FAnalyticsParam VoiceErrorParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("voice"), VoiceErrorValue);
	//const FAnalyticsParam PhotonReconnectErrorParam = UFirebaseFunctionLibrary::CreateStringAnalyticsParameter(TEXT("photon_reconnected"), PhotonReconnectedErrorValue);
	//SEND_FIREBASE_ANALYTICS_WITH_PARAMETERS(TEXT("error"), VoiceErrorParam, PhotonReconnectErrorParam);
}
