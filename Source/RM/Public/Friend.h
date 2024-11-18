// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RMDefinedStruct.h"
#include "GameFramework/SaveGame.h"
#include "Friend.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogFriend, Log, All)


UENUM(BlueprintType)
enum class EFollowState : uint8
{
	ENone		UMETA(ToolTip = "아무 관계 아님"),
	EFollowing	UMETA(ToolTip = "'유저'가 팔로우 중"),
	EFollowee	UMETA(ToolTip = "'상대'가 팔로우 중"),
	EEachOther	UMETA(ToolTip = "서로 팔로우"),
	
};


/**
 *
 */
USTRUCT(Blueprintable, BlueprintType)
struct FSocialInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		int32 UserSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FString Nickname;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FName Thumbnail;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FName Flag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SocialInfo")
		FName countrySP;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		EFollowState FollowState;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		int32 FolloweeCnt;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		int32 FollowerCnt;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FString AboutMe;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FString TodayWord;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FName Grade;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		bool HasBookmark;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FString LevelName;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FString ChannelName;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FDateTime LastLoginTime;
	

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		FPlayerCharacter AvatarInfo;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "SocialInfo")
		bool bHasNewChat = false;
};

USTRUCT(Blueprintable, BlueprintType)
struct FFriendChatMessage
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "FriendChat")
	int32 To;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "FriendChat")
	int32 From;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "FriendChat")
	FString Text;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "FriendChat")
	FDateTime RegDate;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "FriendChat")
	int32 ChatID;

	/* Only used in Game client for draw day info once a day. */
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "FriendChat")
	bool bVisibleDayInfo = false;
};

/**
 *
 */
UCLASS()
class RM_API UFriendFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FriendLibrary")
		static void SetSocialInfo(FString AboutMe, FString TodayWord, FName Thumbnail);
	UFUNCTION(BlueprintCallable, Category = "FriendLibrary")
		static void SetBookmark(int32 UserSeq, bool Marked);
	// UFUNCTION(BlueprintCallable, Category = "FriendLibrary")
	// 	static void Follow(int32 UserSeq);
	// UFUNCTION(BlueprintCallable, Category = "FriendLibrary")
	// 	static void UnFollow(int32 UserSeq);

	
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSocialInfosFinished, int32, ResultCode, const TArray<FSocialInfo>&, Result);

/**
 *
 */
UCLASS()
class RM_API URequestTargetsSocialInfo : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	URequestTargetsSocialInfo() {};

	UPROPERTY(BlueprintAssignable, Category = "Friend")
		FSocialInfosFinished Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static URequestTargetsSocialInfo* RequestTargetsSocialInfo(const TArray<int32>& TargetIDs);
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static URequestTargetsSocialInfo* GetMySocialInfo();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	TArray<int32> TargetIDs;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FFriendInfosFinished, int32, ResultCode, int32, Total, int32, FollowingCount, int32, FollowerCount, const TArray<FSocialInfo>&, Result);

/**
 *
 */
UCLASS()
class RM_API URequestFriendsSocialInfo : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	URequestFriendsSocialInfo() {};

	UPROPERTY(BlueprintAssignable, Category = "Friend")
		FFriendInfosFinished Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static URequestFriendsSocialInfo* RequestFriendsSocialInfo(int32 Page, int32 ShowCnt, bool IsFollowing);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 Page;
	int32 ShowCnt;
	bool IsFollowing;
};


/**
 *
 */
UCLASS()
class RM_API USearchFriends : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	USearchFriends() {};

	UPROPERTY(BlueprintAssignable, Category = "Friend")
		FSocialInfosFinished Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static USearchFriends* SearchFriends(const FString& Keyword);
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static USearchFriends* RequestRecommendedFriends();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString Keyword;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFollowFinished, int32, ResultCode, EFollowState, State);

/**
 *
 */
UCLASS()
class RM_API UFollowFriend : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFollowFriend() {};

	UPROPERTY(BlueprintAssignable, Category = "Friend")
		FFollowFinished Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static UFollowFriend* Follow(int32 UserSeq);
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static UFollowFriend* UnFollow(int32 UserSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 UserSeq;
	bool IsFollow;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTargetPositionFinished, int32, ResultCode, FSocialInfo, SocialInfo);

/**
 *
 */
UCLASS()
class RM_API URequestTargetPosition : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	URequestTargetPosition() {};

	UPROPERTY(BlueprintAssignable, Category = "Friend")
		FTargetPositionFinished Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
		static URequestTargetPosition* RequestTargetPosition(int32 TargetUserSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 TargetUserSeq;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTargetInviteFinished, int32, ResultCode, FSocialInfo, SocialInfo);


UCLASS()
class RM_API URequestTargetInvite : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	URequestTargetInvite() {};

	UPROPERTY(BlueprintAssignable, Category = "Friend")
	FTargetInviteFinished Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
	static URequestTargetInvite* RequestTargetInvite(int32 TargetUserSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 TargetUserSeq;
};




/*
	- ģ�� ä�� (�̹� ��ȸ�� ��� �����ʹ� Ŭ���̾�Ʈ�� ������ �ִ´�.)
	- 0�� Index�������� 20000�� Index���� �������� �ް� �Ľ��ϴµ� ������ S10 5G ���� 1.7�� ���� �ҿ�.
	
	System
	1. FriendChat�� �����ϴ� UObject�� GameInstance�� �̸� ���� �ִ´�. - User CreateAccount�� or GetUserInfoAPI �ߵ� �� �ش� Object ����
		- ��� Chat ����� �ش� Object�� ���ؼ� �̷��� �� �ֵ��� �Ѵ�.
	2. SaveDataObject�� ��� �޽����� ģ�� Key(UserSeq)�� ������ ���� �� �ֵ��� �Ѵ�.
		- saveDataObject�� ��� TMap<int (Key), TArray<FString> (Value)> AllMessages�� ������ �־�� ��.
			> Key�� UserSeq, Value�� �ش� ������ ��ȭ�� ��� ��ϵ� ����. ( Index 0 ~ Index... )
			> AllMessages.Find(Key) == False�� ���, LastChatId�� 0�̸�, True�� ���, AllMessages.At(Key).Last()->ChatID���� �ȴ�.
			> ��ȭ�� ��� ����� Ŭ���̾�Ʈ���� �����ϸ�, �ش� �������� ��ȭ�� ��Ͽ� IsNew�� ���ٸ�, ������ API��û�� �ص� �޽����� �߰����� �����Ƿ� ����.
		- SaveData�� "MyUserSeq" + "_FriendsChatData"�� �Ѵ�.
		- �ȷο�, �ȷ��� �����Ϳ��� �����Ǹ�?... �޽����� �����Ǿ���ϳ�? - �׳� ������ �ִ°ɷ�
	3. Object->Init()�� ���� SaveData�� �ҷ��ͼ� ������ �ִ´�.
	4. User Log Out �� User Account Delete��, Ŭ�� ����� �ش� �����͸� ��� ����ó�� �Ѵ�.
		- �𸮾� ���� �Լ����� ������ ĳ�� �����͸� ������ ����� ������?
		- ���� �����ÿ��� ��� �ؾߵɱ�?
	
	
	Flow
	1. ģ�� ����Ʈ ��ȸ ��, �� ģ������ ���ο� �޽����� �����ϴ��� �˾� �� �� �ִ�. (IsNew�� �˾ƿ� �� ����)
		- ���� ��� â�� ������� �־��� ��� ������ ��� �ؾ��ϴ��� ?
	2. ģ�� List�� ���� ��ư Ŭ�� ��, �����ʿ� ä�� ������ �������� �Ѵ�.
	3. new message�� �����ϴ� ���, URequestFriendsMessage Ŭ������ �̿��Ͽ� API�� ȣ��.
		- 1���� �����ϰ� ���� new�� �ƴ����� ���ο� �޽����� �ִٸ�? -> 1�п� �� ���� �޽����� ��ȸ�Ѵ�.
	4. new message�� ���� ���, ���� �޽��� Array�� LastIndex���� �Ųٷ� 25����? ����Ʈ�� �Ѹ���.
		- 1�� ���� �޽����� ������ ������, �޽��� ����ȸ.
	5. Entry_List�� Object�߰��� text�� 50��(��ȹ���� 50��)�� �����Ѵ�.
	6. FriendChatMessage�� To, From�� �����Ͽ�, ������ ����� ������ �ٸ� ��������� ���� Widget�� Switcher�� �����Ѵ�.
	7. ���� ��ũ�� ��, ���� �޽����� �����ͼ� ����Ʈ �信 �߰��Ѵ�.
	
*/

/** Used for Nested Container.
 * TMap<int, TArray<FFriendChatMessage>>
 **/
USTRUCT(BlueprintType)
struct FChatArray
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FFriendChatMessage> msgArr;
};

UCLASS(Blueprintable, BlueprintType)
class RM_API UUserChatSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	/* Key = UserSeq, Value = AllMessages */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<int, FChatArray> AllMessages;
};


DECLARE_DYNAMIC_DELEGATE_ThreeParams(FFinishedRequestNewChatList, bool, bSucceeded, int32, ResultCode, const TArray<FFriendChatMessage>&, NewMessage);
UCLASS(Blueprintable, BlueprintType)
class RM_API URMFriendObject : public UObject
{
	GENERATED_BODY()
public:
	/** Constructor **/
	URMFriendObject();

	/**
	 * Initializer for Load SaveFile and trigger bIsInit flag as true.
	 * if can't find save file, Create Save File.
	 **/
	UFUNCTION(BlueprintCallable)
	void Init();

	/** Reset this Object. use to reset game. **/
	UFUNCTION(BLueprintCallable)
	void ResetObject();

	/** Save Chat info. **/
	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "Success?")	bool SaveChatData();

	/**
	 * Load Chat info.
	 * if can't find savefiles, create Save File.
	 **/
	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "Success?")	bool LoadChatData();

	UFUNCTION(BlueprintCallable)
	void ClearChatData();

	/** Get All Messages By FriendUserSeq(int32) **/
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "GET"), Category = "FriendObject", DisplayName="GET")
	UPARAM(DisplayName = "Messages")	TArray<FFriendChatMessage> GetChatMessage(int32 FriendUserSeq);

	/**
	 * Get Chat Messages for range.
	 * if LastIndex exceed Messages range, will set Message's last Index.
	 * used for Get Prev ChatMessages.
	 * Range: (LastIndex - Count) ~ LastIndex
	 **/
	UFUNCTION(BlueprintCallable, Category = "FriendObject", DisplayName="GET RANGE")
	UPARAM(DisplayName = "Messages")	TArray<FFriendChatMessage> GetChatMessageForRange(int32 FriendUserSeq, int32 Count, int32& returnStartIndex, int32 LastIndex = -1);

	/**
	 * Request new chat list by friendSeq.
	 * if not existed new chat list, return none.
	 * if player send chat, Object will save in client and server by given unique chatID.
	 **/
	UFUNCTION(BlueprintCallable, Category = "FriendObject")
	void TryRequestNewChatList(int32 FriendUserSeq, FString Text, const FFinishedRequestNewChatList& onCompleteRequest);

	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (exposeOnSpawn="true"), Category = "FriendObject")
	FString SaveFileName;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsInit = false;

private:
	/** used in private
	 * Response Function in 'TryReqeustNewChatList' for http request.
	 * Get Json Data if success.
	 **/
	void OnResponseRequestHttp(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
	void ExecuteHttpRequestFinished(bool bSucceeded, int32 ResultCode, TArray<FFriendChatMessage> NewMessages);
	static inline bool GetVisibilityDayInfo(FDateTime lastMessageDateTime, FDateTime currentMessageDateTime);
private:
	/* AllMessages all follow and following user. */
	UPROPERTY()
	TMap<int, FChatArray> AllMessages;
	
	/* Delegate used for inform finishing http request. */
	UPROPERTY()
	FFinishedRequestNewChatList OnFinishedRequestNewChatLists;

	/* current FriendUserSeq */
	UPROPERTY()
	int32 CurrentFriendSeq = -1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFCMSignFinished, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API URequestFCMSign : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	URequestFCMSign() {}

	UPROPERTY(BlueprintAssignable, Category = "Friend")
	FFCMSignFinished Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Friend")
	static URequestFCMSign* RequestFCMSign(FString FCMToken, FString Lang);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString FCMToken;
	FString Lang;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostFriendRankFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FFriendFollowRankElem>&, FrindRankArray);

UCLASS()
class UPostFriendRank : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostFriendRank() {};

	UPROPERTY(BlueprintAssignable, Category = "FriendRankAPI")
	FPostFriendRankFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "FriendRankAPI")
	static UPostFriendRank* PostFriendRank(const FString SortType, const FString keyword);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
private:
	FString SortType;
	FString keyword;
	
};
