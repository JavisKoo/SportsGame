// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RM/RMProtocolFunctionLibrary.h"
#include "VoteSubsystem.generated.h"

USTRUCT(BlueprintType, Blueprintable, Atomic)
struct FVoteClubPlayerInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubPlayerInfo")
		int ClubSeq = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubPlayerInfo")
		int PlayerSeq = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubPlayerInfo")
		FText PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubPlayerInfo")
		int Sorting = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubPlayerInfo")
		int VoteCnt = 0;
};

USTRUCT(BlueprintType, Blueprintable, Atomic)
struct FVoteClubInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int CvcSeq = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int Club1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int Club1Doing =0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int Club1Vc = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int Club2 = 0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "VoteClubInfo")
		int Club2Doing = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int Club2Vc = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		FText Comment;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		bool doingYn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int Sorting = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		TArray<FVoteClubPlayerInfo> PlayerList;
};

USTRUCT(BlueprintType, Blueprintable, Atomic)
struct FVoteClubPlayerSprite : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VotePlayerInfo")
		int PlayerSeq = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VotePlayerInfo")
		int ClubSeq = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VotePlayerInfo")
		FText PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VotePlayerInfo")
		UPaperSprite* PlayerTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VotePlayerInfo")
		UPaperSprite* PlayerRoundedSSumNail;

};

USTRUCT(BlueprintType, Blueprintable, Atomic)
struct FVoteClubSprite : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		int ClubSeq = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		FText ClubName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		UPaperSprite* ClubTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteClubInfo")
		UPaperSprite* ClubRoundedSSumNail;
};

USTRUCT(BlueprintType, Blueprintable, Atomic)
struct FVoteComment
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteComment")
		int Seq = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteComment")
		int VoteSeq = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteComment")
		int PlayerSeq = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteComment")
		FString Contents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteComment")
		FDateTime RegDate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoteComment")
		FString NickName;
};

UCLASS()
class RM_API UVoteBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = ""), Category = "VoteBlueprintFunctionLibrary")
		static UPARAM(DisplayName = "") FString ToString(FVoteClubInfo VoteListInfo);
};

/*
 * 구현해야할 함수
 * 1. Initialize
 * - DataTable에서 모든 선수 이름 및 Texture 가져오기
 * - DataTable에서 모든 선수 이름으로 Hash Table 생성 (VoteInfo)
 * 2. HTTP REQUEST
 * - Server와 통신 후, VoteInfo 얻기
 *		> 플레이어 이름으로 Hash Table(VoteInfo) 채워넣기
 *			> 1) PlayerName
 *			> 2) VoteCount
 *			> 3) Rank
 *			> 4) SpriteTexture
 * - Server와 통신 후, ChatInfo 얻기
 *		> RequestPlayerVoteChatInfo(FString PlayerName, int LastSequence, int Count)
 *		> PlayerName에 해당하는 LastSequence 이후의 Count개 댓글을 가져온다.
 *		> Subsystem에서는 데이터를 뿌려주기만 하는 Http 통신만 한다.
 *		> 해당 댓글은 Widget에서 사용 후, Refresh시 Clear 되고 다시 받아온다.
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateVoteList);

UCLASS()
class RM_API UVoteSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/* Constructor Initializer */
	UVoteSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* HTTP Request Function */
	/**
	 * 현재 투표 리스트를 요청합니다.
	 * @param bDoingYn - 경기가 현재 진행중인지 확인합니다 (0 : 전체경기, 1 : 현재경기)
	 * @param OnResponse RequestCode 200 - 성공시 실행될 델리게이트입니다.
	 * @param OnError RequestCode Else - 실패시 실행될 델리게이트입니다.
	 */
	UFUNCTION(BlueprintCallable)
	void RequestClubVoteList(bool bDoingYn, const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError);
	
	/**
	 * 현재 투표 리스트를 요청합니다.
	 * @param cvcSeq - 현재 진행 중인 경기의 시퀀스입니다
	 * @param club1 - 첫번째 클럽시퀀스
	 * @param club2 - 두번째 클럽시퀀스
	 * @param OnResponse RequestCode 200 - 성공시 실행될 델리게이트입니다.
	 * @param OnError RequestCode Else - 실패시 실행될 델리게이트입니다.
	 */
	UFUNCTION(BlueprintCallable)
		void RequestProcessClubVote(int cvcSeq, int club1, int club2, const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError);
	

	/**
	 * 투표를 진행합니다.
	 * @param cvcSeq 현재 진행 중인 경기의 시퀀스입니다. 
	 * @param PlayerSeq 투표될 플레이어(선수)의 시퀀스입니다. (정확히 3명의 선수를 인자로 넣어야 합니다.)
	 * @param OnResponse RequestCode 200 - 성공시 실행될 델리게이트입니다.
	 * @param OnError RequestCode Else - 실패시 실행될 델리게이트입니다.
	 */
	UFUNCTION(BlueprintCallable)
		void RequestProcessPlayerVote(int cvcSeq, TArray<int> PlayerSeq, int clubSeq,const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError);

	/**
	 * 투표 댓글 리스트를 요청합니다. (성공시 댓글 갯수는 10개입니다.)
	 * @param cvcSeq 현재 진행중인 경기의 시퀀스입니다.
	 * @param PlayerSeq 가져올 댓글의 플레이어(선수)의 시퀀스입니다.
	 * @param LastCommentSeq 현재 가지고 있는 마지막 댓글 시퀀스입니다. (0이면 모든 댓글 리스트를 Refresh합니다.)
	 * @param OnResponse RequestCode 200 - 성공시 실행될 델리게이트입니다.
	 * @param OnError RequestCode Else - 실패시 실행될 델리게이트입니다.
	 */
	UFUNCTION(BlueprintCallable)
		void RequestCommentList(int cvcSeq, int PlayerSeq, int LastCommentSeq, const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError);

	/**
	* 투표 댓글 추가를 요청합니다. (성공시 댓글 갯수는 10개입니다.)
	* @param cvcSeq 현재 진행중인 경기의 시퀀스입니다.
	* @param PlayerSeq 가져올 댓글의 플레이어(선수)의 시퀀스입니다.
	* @param Comment 댓글로 넣을 내용입니다.
	* @param OnResponse RequestCode 200 - 성공시 실행될 델리게이트입니다.
	* @param OnError RequestCode Else - 실패시 실행될 델리게이트입니다.
	*/
	UFUNCTION(BlueprintCallable)
		void RequestCommentPush(int cvcSeq, int PlayerSeq, FString Comment, const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError);

	/**
	 * 가지고 있는 코멘트 데이터 중 가장 최근에 추가된 코멘트를 가져옵니다.
	 * @param Count 가져올 댓글의 숫자입니다.
	 * @param LastCommentSeq Count개를 추릴 때 사용할 댓글의 시퀀스 값입니다.
	 *						 만약 Count개의 댓글 중 해당 Seq가 포함되었다면, Seq 댓글이 포함된 이후 댓글들은 리턴하지 않습니다. (기본 값은 -1입니다.)
	 * @return VoteSubsystem::CurrentCommentList의 최신 Count개의 댓글을 리턴합니다.
	 */
	UFUNCTION(BlueprintCallable)
		TArray<FVoteComment> GetLatestComments(int Count, int LastCommentSeq = 0);

private:
	/* Private Function For Implement Http Request Succeeded Lambda. */
	bool SetVoteListData(const TSharedPtr<FJsonObject> Data);
	bool SetCommentListData(const TSharedPtr<FJsonObject> Data);
public:
	/* Vote Container */
	UPROPERTY(BlueprintReadOnly, Category = "VoteSubsystem")
		TMap<int, UPaperSprite*> PlayerTextureInfo;
	UPROPERTY(BlueprintReadOnly, Category = "VoteSubSystem")
		TMap<int, FVoteClubInfo> VoteClubListInfoTable;
	/* Comment Container */
	UPROPERTY(BlueprintReadOnly, Category = "VoteSubsystem")
		TArray<FVoteComment> CurrentCommentList;
private:
	/* Dynamic Delegate */
	/* Vote Delegate*/
	UPROPERTY()
		FOnHttpRequestResponseDelegate OnRequestVoteInfoFinished;
	UPROPERTY()
		FOnHttpRequestErrorDelegate OnRequestVoteInfoError;
	UPROPERTY()
		FOnHttpRequestResponseDelegate OnRequestProcessVoteFinished;
	UPROPERTY()
		FOnHttpRequestErrorDelegate OnRequestProcessVoteError;
	/* Update Vote List Multicast Delegate */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnUpdateVoteList OnUpdateVoteList;

	/* Comment Delegate */
	UPROPERTY()
		FOnHttpRequestResponseDelegate OnRequestVoteCommentListFinished;
	UPROPERTY()
		FOnHttpRequestErrorDelegate OnRequestVoteCommentListError;
	UPROPERTY()
		FOnHttpRequestResponseDelegate OnRequestPushVoteCommentFinished;
	UPROPERTY()
		FOnHttpRequestErrorDelegate OnRequestPushVoteCommentError;

	/* Data Table */
	UPROPERTY()
		UDataTable* DT_VotePlayerSpriteTable;
};