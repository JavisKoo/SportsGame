// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HttpModule.h"
#include "Engine/DataTable.h"
//#include "QuestSaveGame.h"
#include "RMDefinedEnum.h"
#include "RM/RMStruct.h"
#include "QuestSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuest, Log, All)

/**
 * 
 */
UENUM(BlueprintType)
enum class EMissionType : uint8
{
	None,
	Quiz,
	Emoticon		UMETA(ToolTip = "(EMapType, 11_EmoticonTable.Row)"),
	Gesture			UMETA(ToolTip = "(EMapType, 12_GestureTable.Row)"),
	ContentCheck	UMETA(ToolTip = "(EMapType.None, 04_ContentTable.Row)"),
	Chat			UMETA(ToolTip = "(EMapType)"),
	VoiceChat		UMETA(ToolTip = "(EMapType)"),
	CreateRoom		UMETA(ToolTip = "(EMapType)"),
	JoinDefaultRoom	UMETA(ToolTip = "(EMapType, RoomName)"),
	JoinUserRoom	UMETA(ToolTip = "(EMapType)"),
};


UENUM(BlueprintType)
enum class EQuestState : uint8
{
	None,
	Successful		UMETA(ToolTip = "used in quiz"),
	Failed			UMETA(ToolTip = "used in quiz"),
	Completed,
};


USTRUCT(BlueprintType)
struct FQuestState
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuestState")
		FName QuestID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuestState")
		EQuestState State = EQuestState::None;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestState")
		EMissionType MissionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestState")
		EMapType MissionPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestState")
		FString MissionValue;
};


USTRUCT(Blueprintable)
struct FQuizQuestTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quiz")
		int32 ResultNo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quiz")
		FString Desc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quiz")
		FText Val1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quiz")
		FText Val2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quiz")
		FText Val3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quiz")
		FText Val4;
};

USTRUCT(Blueprintable)
struct FDailyQuestTable : public FTableRowBase	// FDailyQuestTableRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		FText Desc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		FText Hint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		EItemType RewardType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		int32 RewardValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		EMissionType MissionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		EMapType MissionPlace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		FString MissionValue;
};


USTRUCT(Blueprintable)
struct FTicketRewardTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TicketReward")
		int32 TicketSeq;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TicketReward")
		int32 Total;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TicketReward")
		FString TicketCode;
	
};

USTRUCT(Blueprintable)
struct FAttendanceInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTicketRewardTable> ATTENDANCE_1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTicketRewardTable> ATTENDANCE_2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTicketRewardTable> ATTENDANCE_3;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTicketRewardTable> ATTENDANCE_4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTicketRewardTable> ATTENDANCE_5;

	void FromObject(TSharedPtr<FJsonObject> JsonObject);
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangedQuest, bool, HasSuccessfulQuest, EQuestState, NewState);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedQuest, bool, HasSuccessfulQuest);

DECLARE_DYNAMIC_DELEGATE_OneParam(FAttendanceResponseDelegate, FString, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAttendanceErrorDelegate,int32, ErrorCode);

UCLASS()
class RM_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UQuestSubsystem();

	UFUNCTION(BlueprintCallable)
	void Request_GetTicketReward(const FAttendanceErrorDelegate& ErrorDelegate, const FAttendanceResponseDelegate& ResponseDelegate);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void Init();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "QuestSubSystem")
		TArray<FQuestState> GetDailyQuestList() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "QuestSubSystem")
		bool HasSuccessfulQuest();

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		bool CompleteQuest(FName QuestID);

#pragma region Mission
	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void DoEmoticon(EMapType Place, FName RowName);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void DoGesture(EMapType Place, FName RowName);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void DoContentCheck(FName RowName);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void DoChat(EMapType Place);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void DoVoiceChat(EMapType Place);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void CreateRoom(EMapType Place);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void JoinUserRoom(EMapType Place);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		void JoinDefaultRoom(EMapType Place, FString RoomName);

	UFUNCTION(BlueprintCallable, Category = "QuestSubSystem")
		EQuestState SolveQuiz(FName QuestID, bool IsSuccessful);
#pragma endregion

private:
	void Request_DailyQuests();
	void Response_DailyQuests(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	void Request_GetQuestReward(FName QuestID);
	void Response_GetQuestReward(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
	
	void CheckMission(EMissionType MissionType, EMapType Place);
	void CheckMission(EMissionType MissionType, EMapType Place, FString Value);

	void DecideeQuest(TArray<FName>& ServerList);
	void SaveGame();

private:
	class UDataTable* DailyDataTable = nullptr;

	const FString QuestSaveSlotName = TEXT("Quest");



	TArray<FQuestState> Daily;
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FAttendanceInfo AttendanceInfo;

	FAttendanceResponseDelegate OnResponse;
	FAttendanceErrorDelegate OnError;
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "QuestSubSystem")
		FOnChangedQuest OnChangedQuestDelegate;
};
