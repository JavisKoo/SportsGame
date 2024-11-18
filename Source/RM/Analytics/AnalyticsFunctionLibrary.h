// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebaseFunctionLibrary.h"
#include "AnalyticsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RM_API UAnalyticsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Utility Function for using in Blueprint.

	/**
	 * Get Used Firebase Plugins for sending firebase PushNotification Message.
	 * @return if currentBundleType is Default(Use Firebase), return true. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMAnalytics|Utility", meta = (CompactNodeTitle = "Enable Firebase Analytics"))
	static UPARAM(DisplayName = "Enabled?") bool CanSendMessage();
	
	/**
	 * Add Param in Analytics Array For Return Value as Array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMAnalytics|Utility", meta = (CompactNodeTitle = "ADD"))
	static UPARAM(displayName = "OutParamArray") TArray<FAnalyticsParam> AddAnalyticsParamArray(TArray<FAnalyticsParam> ParamArray, FAnalyticsParam AddedParam);

	/**
	 * Append ParamArray in Analytics Array For Return Value as Array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMAnalytics|Utility", meta = (CompactNodeTitle = "APPEND"))
	static UPARAM(displayName = "OutParamArray") TArray<FAnalyticsParam> AppendAnalyticsParamArray(TArray<FAnalyticsParam> ParamArray, TArray<FAnalyticsParam> Appended);

	/**
	 * Create analytics parameter array.
	 * Default zero index param is StringParameter. (Key: UserId, Value: URMProtocolFunctionLibrary::GetUserSeq())
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMAnalytics|CreateParameter")
	static UPARAM(displayName = "AnalyticsParamArray") TArray<FAnalyticsParam> CreateDefaultAnalyticsParameterArray(); 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMAnalytics|CreateParameter")
	static UPARAM(displayName = "OutArray") TArray<FAnalyticsParam> CreateDefaultAnalyticsClickEventParameterArray();

	/**
	 * To create frequently used analytics parameter values (SpentTime Param)
	 * @param EnterTime To calculate stay time. The incoming time must be less than UtcNow(), and the difference must be within 48 hours. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMAnalytics|CreateParameter")
	static UPARAM(displayName = "OutParam") FAnalyticsParam CreateSpentTimeAnalyticsParameter(FDateTime EnterTime);

	/**
 	 * To create frequently used analytics parameter values (Click Param)
 	 * @param Click Send Click event (0 = NULL, 1 = onClick) 
 	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RMAnalytics|CreateParameter")
	static UPARAM(displayName = "OutParam") FAnalyticsParam CreateClickAnalyticsParameter(int64 Click = 0);

	
public:
	// SEND Method for query Firebase Analytics.

	/**
  	 * Send QuickSlot Analytics event. (QuickSlot_Slot_Main)
  	 * @param Click To send Click event (0 = NULL, 1 = onClick)
  	 * @param Menu The name of the clicked button type
  	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|UI")
	static void SendAnalyticsEvent_Sidebar(bool Click, FString Menu);

	/**
	 * Send UserList Analytics event. (See Btn_Friend)
	 * @param Click To Send Click event (0 = NULL, 1 = onClick)
	 * @param FollowStatus To Send FollowStatus with friend after click follow button. (follow or unfollow)
	 * @param EnterTime To Send the time the user list is displayed on the screen.
	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|UI")
	static void SendAnalyticsEvent_UserListButton(bool Click, FString FollowStatus, FDateTime EnterTime);

	/**
 	 * Send Emogi Analytics event. (See EmoticonList)
 	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Chat")
	static void SendAnalyticsEvent_EmogiButton(FString EmogiID);

	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Chat")
	static void SendAnalyticsEvent_Test_Button(FString TestID);


	/**
  	 * Send Gesture Analytics event. (See Gesture)
  	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Chat")
	static void SendAnalyticsEvent_GestureButton(FString GestureID);

	/**
   	 * Send Level Analytics event, when user leave the room. (See RMGameInstance Map Loader Event Graph)
   	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Level")
	static void SendAnalyticsEvent_Level(FString LevelName, FString RoomType, FDateTime EnterTime);

	/**
	* Send Market showing Analytics event.
	*/
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Market")
	static void SendAnalyticsEvent_MarketButton(bool Click);
	
	/**
	* Send Costume shop analytics event.
	* @param Click Send when user click the item.
	* @param Buy Send when user purchase the item.
	* @param Type purchased item type.
	* @param ItemID purchased item id.
	* @param EnterTime calculate spentTime.
	*/
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Costume")
	static void SendAnalyticsEvent_Costume(bool Click, bool Buy, FString Type, FString ItemID, FDateTime EnterTime);

	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|EventBoard")
	static void SendAnalyticsEvent_BoardEvent(bool Click, FString Type, FString EventCode);

	/**
	 * Send MiniGame event.
	 * @param Type minigame type value.
	 * @param EnterTime calculate spentTime.
	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|MiniGame")
	static void SendAnalyticsEvent_MiniGame(FString Type, FDateTime EnterTime);

	/**
 	 * Send MiniGame event.
 	 * @param bIsCreate actionType(Cancel or Create)
 	 * @param Type Type value (TOUR, VIP, COMMUNITY)
 	 * @param LevelName map name sending name.
 	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Vip_Community")
	static void SendAnalyticsEvent_CreateRoom(bool bIsCreate, FString Type, FString LevelName);

	/**
	 * Send Additional Download event when user cancel the Coin of Membershio popup.
	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|System")
	static void SendAnalyticsEvent_CancelAdditionalDownloads(bool bIsCanceled = true);

	/**
 	 * Send Error Message into bigquery.
 	 * @param VoiceErrorValue (NewGameMode::onSTT error)
 	 * @param PhotonReconnectedErrorValue (Photon Object Reconnect error popup.)
 	 */
	UFUNCTION(BlueprintCallable, Category = "RMAnalytics|Send|Error")
	static void SendAnalyticsEvent_Error(FString VoiceErrorValue, FString PhotonReconnectedErrorValue);
};
