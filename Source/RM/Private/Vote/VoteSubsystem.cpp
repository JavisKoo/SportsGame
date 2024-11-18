// Fill out your copyright notice in the Description page of Project Settings.


#include "Vote/VoteSubsystem.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Algo/Reverse.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/KismetArrayLibrary.h"

#define CHECK_ACCESS_TOKEN																						\
		if (URMProtocolFunctionLibrary::GetAccessToken() == TEXT(""))											\
			URMProtocolFunctionLibrary::SetAccessToken(TEXT("eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJxUUo0Y09lU2JnYkF5ZzBxVjZ6bjdKM2dGSEMyIiwiYXV0aCI6IlJPTEVfVVNFUiIsInNlcSI6MjE5NTU0LCJzZXNzaW9uIjoiVE9RV3R0RUt2NkUzZGJuZmJHOTJvN0hzdzEtR19EckZwMGxqMTNibyIsImV4cCI6MTY3MjI1Mzg1MX0.gNYYlTJltAGDho4Yj-HxnRhRVlVgaVg6wa7fAeDfUFK0CBwzTzaif1NMzZoUWGAGSTSmL2c-Y_kcVncAh5c_HA"))

FString UVoteBlueprintFunctionLibrary::ToString(FVoteClubInfo VoteListInfo)
{
	FString Out;
	FJsonObjectConverter::UStructToJsonObjectString(VoteListInfo, Out);
	return Out;
}

UVoteSubsystem::UVoteSubsystem()
{
	//static ConstructorHelpers::FObjectFinder<UDataTable> PlayerSpriteTable(TEXT("DataTable'/Game/Qatar/DataTable/VotePlayerSpriteTable.VotePlayerSpriteTable'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> PlayerSpriteTable(TEXT("DataTable'/Game/Qatar/DataTable/VoteClubPlayerSpriteTable.VoteClubPlayerSpriteTable'"));
	if (PlayerSpriteTable.Succeeded())
		DT_VotePlayerSpriteTable = PlayerSpriteTable.Object;
}

void UVoteSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (DT_VotePlayerSpriteTable)
	{
		UE_LOG(LogTemp, Log, TEXT("DT_VotePlayerSpriteTable is Valid"));
		TArray<FVoteClubPlayerSprite*> SpritesArray;
		DT_VotePlayerSpriteTable->GetAllRows(TEXT(""), SpritesArray);
		for (const FVoteClubPlayerSprite* SpriteInfo : SpritesArray)
		{
			if (SpriteInfo)
			{
				int key = SpriteInfo->PlayerSeq;
				UPaperSprite* val = SpriteInfo->PlayerTexture;
				PlayerTextureInfo.Add(key, val);
			}
		}
	}
}

void UVoteSubsystem::RequestClubVoteList(bool bDoingYn, const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError)
{
	CHECK_ACCESS_TOKEN;
	OnRequestVoteInfoFinished = OnResponse;
	OnRequestVoteInfoError = OnError;

	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("vote/list"), TEXT("POST"));
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("doing", 1);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload); 
	URMProtocolFunctionLibrary::ProcessDefaultHttp(HttpRequest, OnRequestVoteInfoFinished, OnRequestProcessVoteError, [&](TSharedPtr<FJsonObject> res)
	{	// Start Lambda
			const TSharedPtr<FJsonObject> Data = res->GetObjectField("data");
		if (SetVoteListData(Data))
		{
		// Set Here to Log.
			UE_LOG(LogRMProtocol, Warning, TEXT("SUCCEEDED - %s"), *((FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))));
		}
		FText OutVal;
		FJsonObjectConverter::GetTextFromObject(res.ToSharedRef(), OutVal);
		EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestResponseDelegate, OnRequestVoteInfoFinished, OutVal.ToString());
		OnUpdateVoteList.Broadcast();
	});	// End Lambda
}

void UVoteSubsystem::RequestProcessClubVote(int cvcSeq, int club1, int club2, const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError)
{
	CHECK_ACCESS_TOKEN;
	OnRequestProcessVoteFinished = OnResponse;
	OnRequestProcessVoteError = OnError;

	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("vote/club"), TEXT("PUT"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("cvcSeq", cvcSeq);
	_payloadJson->SetNumberField("club1", club1);
	_payloadJson->SetNumberField("club2", club2);
	
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	URMProtocolFunctionLibrary::ProcessDefaultHttp(HttpRequest, OnRequestProcessVoteFinished, OnRequestProcessVoteError, [&](TSharedPtr<FJsonObject> res)
	{
		//start Lambda
		const TSharedPtr<FJsonObject> Data = res->GetObjectField("data");
		if (SetVoteListData(Data))
		{
			//Set Here to go
			UE_LOG(LogRMProtocol, Warning, TEXT("Succeeded - &s"), *((FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))));

		}
		FText OutVal;
		FJsonObjectConverter::GetTextFromObject(res.ToSharedRef(), OutVal);
		EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestResponseDelegate, OnRequestProcessVoteFinished, OutVal.ToString());
		OnUpdateVoteList.Broadcast();
	});
}

void UVoteSubsystem::RequestProcessPlayerVote(int cvcSeq, TArray<int> PlayerSeq, int clubSeq,const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError)
{
	CHECK_ACCESS_TOKEN;
	OnRequestProcessVoteFinished = OnResponse;
	OnRequestProcessVoteError = OnError;

	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("vote/player"), TEXT("PUT"));

	TArray<TSharedPtr<FJsonValue>> PlayerSeqJsonArr;
	for (int seq : PlayerSeq)
		PlayerSeqJsonArr.Add(MakeShareable(new FJsonValueNumber(seq)));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("cvcSeq", cvcSeq);
	_payloadJson->SetNumberField("clubSeq", clubSeq);
	_payloadJson->SetArrayField("playerSeq", PlayerSeqJsonArr);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	URMProtocolFunctionLibrary::ProcessDefaultHttp(HttpRequest, OnRequestProcessVoteFinished, OnRequestProcessVoteError, [&](TSharedPtr<FJsonObject> res)
	{
		//start Lambda
		const TSharedPtr<FJsonObject> Data = res->GetObjectField("data");
		if (SetVoteListData(Data))
		{
			//Set Here to go
			UE_LOG(LogRMProtocol, Warning, TEXT("Succeeded - &s"), *((FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))));

		}
		FText OutVal;
		FJsonObjectConverter::GetTextFromObject(res.ToSharedRef(), OutVal);
		EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestResponseDelegate, OnRequestProcessVoteFinished, OutVal.ToString());
		OnUpdateVoteList.Broadcast();
	});
}

void UVoteSubsystem::RequestCommentList(int cvcSeq, int PlayerSeq, int LastCommentSeq,
	const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError)
{
	CHECK_ACCESS_TOKEN;
	OnRequestVoteCommentListFinished = OnResponse;
	OnRequestVoteCommentListError = OnError;
	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("vote/comment-list"), TEXT("POST"));
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("cvcSeq", cvcSeq);
	_payloadJson->SetNumberField("playerSeq", PlayerSeq);
	_payloadJson->SetNumberField("seq", LastCommentSeq);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	URMProtocolFunctionLibrary::ProcessDefaultHttp(HttpRequest, OnRequestVoteCommentListFinished, OnRequestVoteCommentListError, [&, LastCommentSeq](TSharedPtr<FJsonObject> res)
		{	// Start Lambda
			const TSharedPtr<FJsonObject> Data = res->GetObjectField("data");
	if (LastCommentSeq == 0)
		CurrentCommentList.Empty();
	if (SetCommentListData(Data))
	{
		// Set Here to Log.
		UE_LOG(LogRMProtocol, Warning, TEXT("SUCCEEDED - %s"), *((FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))));
	}
	FText OutVal;
	FJsonObjectConverter::GetTextFromObject(res.ToSharedRef(), OutVal);
	EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestResponseDelegate, OnRequestVoteCommentListFinished, OutVal.ToString());
		});	// End Lambda
}

void UVoteSubsystem::RequestCommentPush(int cvcSeq, int PlayerSeq, FString Comment,
	const FOnHttpRequestResponseDelegate& OnResponse, const FOnHttpRequestErrorDelegate& OnError)
{
	CHECK_ACCESS_TOKEN;
	OnRequestPushVoteCommentFinished = OnResponse;
	OnRequestPushVoteCommentError = OnError;
	const auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("vote/comment"), TEXT("PUT"));
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetNumberField("cvcSeq", cvcSeq);
	_payloadJson->SetNumberField("playerSeq", PlayerSeq);
	_payloadJson->SetStringField("contents", Comment);
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	URMProtocolFunctionLibrary::ProcessDefaultHttp(HttpRequest, OnRequestPushVoteCommentFinished, OnRequestPushVoteCommentError, [&](TSharedPtr<FJsonObject> res)
		{	// Start Lambda
			const TSharedPtr<FJsonObject> Data = res->GetObjectField("data");
	CurrentCommentList.Empty();
	if (SetCommentListData(Data))
	{
		// Set Here to Log.
		UE_LOG(LogRMProtocol, Warning, TEXT("SUCCEEDED - %s"), *((FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))));
	}
	FText OutVal;
	FJsonObjectConverter::GetTextFromObject(res.ToSharedRef(), OutVal);
	EXECUTE_DYNAMIC_DELEGATE(FOnHttpRequestResponseDelegate, OnRequestPushVoteCommentFinished, OutVal.ToString());
		});	// End Lambda
}

TArray<FVoteComment> UVoteSubsystem::GetLatestComments(int Count, int LastCommentSeq)
{
	TArray<FVoteComment> Out;
	for (int i = CurrentCommentList.Num() - 1; i >= 0; i--)
	{
		FVoteComment Added = CurrentCommentList[i];
		if (Added.Seq == LastCommentSeq || Count <= 0)
			break;
		--Count;
		Out.Add(Added);
	}
	Algo::Reverse(Out);
	return Out;
}

bool UVoteSubsystem::SetVoteListData(const TSharedPtr<FJsonObject> Data)
{
	if (Data.Get() != nullptr)
	{
		const TArray<TSharedPtr<FJsonValue>> List = Data->GetArrayField(TEXT("voteList"));
		for (TSharedPtr<FJsonValue> Val : List)
		{
			TSharedPtr<FJsonObject> Obj = Val->AsObject();
			FVoteClubInfo ClubListInfo;
			FJsonObjectConverter::JsonObjectToUStruct(Obj.ToSharedRef(), &ClubListInfo);

			if (VoteClubListInfoTable.Contains(ClubListInfo.CvcSeq))
				VoteClubListInfoTable[ClubListInfo.CvcSeq] = ClubListInfo;
			else
				VoteClubListInfoTable.Add(ClubListInfo.CvcSeq, ClubListInfo);
		}
		return true;
	}
	return false;
}

bool UVoteSubsystem::SetCommentListData(const TSharedPtr<FJsonObject> Data)
{
	if (Data.Get() != nullptr)
	{
		TArray<TSharedPtr<FJsonValue>> CommentLists = Data->GetArrayField("commentList");
		for (TSharedPtr<FJsonValue> List : CommentLists)
		{
			TSharedPtr<FJsonObject> Obj = List->AsObject();
			FVoteComment Comment;
			FJsonObjectConverter::JsonObjectToUStruct(Obj.ToSharedRef(), &Comment);
			FDateTime::Parse(Obj->GetStringField("regdate"), Comment.RegDate);
			CurrentCommentList.Add(Comment);
		}
		return true;
	}

	return false;
}
