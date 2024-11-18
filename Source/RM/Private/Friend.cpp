// Fill out your copyright notice in the Description page of Project Settings.


#include "Friend.h"
#include "Http.h"
#include "RM/RMProtocolFunctionLibrary.h"
#include "RM/RM_Singleton.h"
#include "Kismet/GameplayStatics.h"
#include "RM/RMBlueprintFunctionLibrary.h"
#include "Engine.h"
#include "MovieSceneTrack.h"

DEFINE_LOG_CATEGORY(LogFriend)


void UFriendFunctionLibrary::SetSocialInfo(FString AboutMe, FString TodayWord, FName Thumbnail)
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("user/me"), TEXT("PATCH"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("aboutMe", AboutMe);
	_payloadJson->SetStringField("todayWord", TodayWord);
	_payloadJson->SetStringField("thumbnail", Thumbnail.ToString());

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	UE_LOG(LogFriend, Log, TEXT("SetSocialInfo : %s"), *_payload);

	HttpRequest->ProcessRequest();
}

void UFriendFunctionLibrary::SetBookmark(int32 UserSeq, bool Marked)
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("friends/favorites"), Marked ? TEXT("POST") : TEXT("DELETE"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("userSeq", FString::FromInt(UserSeq));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	UE_LOG(LogFriend, Log, TEXT("SetBookmark : %s : %s"), *_payload, Marked ? TEXT("true") : TEXT("false"));

	HttpRequest->ProcessRequest();
}

//void RequestFollow(int32 UserSeq, bool IsFollow)
//{
//	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("friends/follow"), IsFollow ? TEXT("POST") : TEXT("DELETE"));
//
//	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
//	_payloadJson->SetStringField("userSeq", FString::FromInt(UserSeq));
//
//	FString _payload;
//	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
//	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
//	HttpRequest->SetContentAsString(_payload);
//
//	HttpRequest->ProcessRequest();
//}

//void UFriendFunctionLibrary::Follow(int32 UserSeq)
//{
//	RequestFollow(UserSeq, true);
//}

//void UFriendFunctionLibrary::UnFollow(int32 UserSeq)
//{
//	RequestFollow(UserSeq, false);
//}

// --------------------------------------------------------------------------------------------------------------------------------
void ToFollowCount(const TSharedPtr<FJsonObject> MyInfo, int32& FllowingCount, int32& FollowerCount)
{
	TSharedPtr<FJsonObject> UserInfo = MyInfo->GetObjectField("userInfo");
	FllowingCount = UserInfo->GetIntegerField("followeeCnt");
	FollowerCount = UserInfo->GetIntegerField("followerCnt");
}

void ToSocialInfo(const TArray<TSharedPtr<FJsonValue>> users, TArray<FSocialInfo>& Result)
{
	Result.Empty();

	for (TSharedPtr<FJsonValue> UserJson : users)
	{
		FSocialInfo info;

		TSharedPtr<FJsonObject> user = UserJson->AsObject();
		TSharedPtr<FJsonObject> base = user->GetObjectField("userInfo");
		info.UserSeq = base->GetIntegerField("userSeq");
		info.Nickname = base->GetStringField("nickName");
		info.Thumbnail = FName(base->GetStringField("thumbnail"));
		info.Flag = FName(base->GetStringField("flagIcon"));
		info.countrySP = FName(base->GetStringField("countrySP"));
		info.HasBookmark = base->GetBoolField("favorites");
		info.FollowState = (EFollowState)base->GetIntegerField("follow");
		info.FolloweeCnt = base->GetIntegerField("followeeCnt");
		info.FollowerCnt = base->GetIntegerField("followerCnt");
		if (base->HasField("aboutMe"))
			info.AboutMe = base->GetStringField("aboutMe");
		if (base->HasField("todayWord"))
			info.TodayWord = base->GetStringField("todayWord");
		info.Grade = FName(base->GetStringField("rank"));
		if (base->HasField("newChat"))
			info.bHasNewChat = (base->GetStringField("newChat") == TEXT("1")) ? true : false;
		else
			info.bHasNewChat = false;
		
		if (user->HasField("userPosition"))
		{
			TSharedPtr<FJsonObject> position = user->GetObjectField("userPosition");
			if (position->HasField("room"))
			{
				info.LevelName = position->GetStringField("room");
				info.ChannelName = position->GetStringField("channel");
			}
			FDateTime::Parse(position->GetStringField("lastLoginDate"), info.LastLoginTime);
		}

		info.AvatarInfo = URM_Singleton::GetDefaultCharacter();

		TSharedPtr<FJsonObject> avatar = user->GetObjectField("customAvatarInfo");
		TSharedPtr<FJsonObject> baseInfo = avatar->GetObjectField(TEXT("baseInfo"));
		//info.AvatarInfo.Gender = baseInfo->GetIntegerField("gender") == 0 ? EGenderRM::Male : EGenderRM::Female;
		if (baseInfo->GetIntegerField("gender") == 0){info.AvatarInfo.Gender = EGenderRM::Male;}
		else if (baseInfo->GetIntegerField("gender") == 1){info.AvatarInfo.Gender = EGenderRM::Female;}
		else if (baseInfo->GetIntegerField("gender") == 2){info.AvatarInfo.Gender = EGenderRM::Male2;}
		else if (baseInfo->GetIntegerField("gender") == 3){info.AvatarInfo.Gender = EGenderRM::Female2;}
		else if (baseInfo->GetIntegerField("gender") == 4){info.AvatarInfo.Gender = EGenderRM::Male3;}
		else if (baseInfo->GetIntegerField("gender") == 5){info.AvatarInfo.Gender = EGenderRM::Female3;}
		else if (baseInfo->GetIntegerField("gender") == 6){info.AvatarInfo.Gender = EGenderRM::Male4;}
		else if (baseInfo->GetIntegerField("gender") == 7){info.AvatarInfo.Gender = EGenderRM::Female4;}
		info.AvatarInfo.TopSize = FCString::Atof(*baseInfo->GetStringField("topSize"));
		info.AvatarInfo.BottomSize = FCString::Atof(*baseInfo->GetStringField("bottomSize"));
		info.AvatarInfo.CheekSize = FCString::Atof(*baseInfo->GetStringField("cheekSize"));
		info.AvatarInfo.Scale = FCString::Atof(*baseInfo->GetStringField("tallScale"));
		info.AvatarInfo.HeadScale = FCString::Atof(*baseInfo->GetStringField("headScale"));
		info.AvatarInfo.HandScale = FCString::Atof(*baseInfo->GetStringField("handScale"));
		info.AvatarInfo.SkinColor = FColor::FromHex(*baseInfo->GetStringField("skinColor"));

		TSharedPtr<FJsonObject> costumeInfo = avatar->GetObjectField(TEXT("costumeInfo"));
		info.AvatarInfo.Costume[ECostumePart::Head] = FName(*costumeInfo->GetStringField("head"));
		info.AvatarInfo.Costume[ECostumePart::Hair] = FName(*costumeInfo->GetStringField("hair"));
		info.AvatarInfo.Costume[ECostumePart::Top] = FName(*costumeInfo->GetStringField("top"));
		info.AvatarInfo.Costume[ECostumePart::Bottom] = FName(*costumeInfo->GetStringField("bottom"));
		info.AvatarInfo.Costume[ECostumePart::Shoes] = FName(*costumeInfo->GetStringField("shoes"));
		// info.AvatarInfo.Costume[ECostumePart::FaceSkin] = FName(*costumeInfo->GetStringField("faceSkin"));
		//info.AvatarInfo.Costume[ECostumePart::EyeBrow] = FName(*costumeInfo->GetStringField("eyeBrow"));
		//info.AvatarInfo.Costume[ECostumePart::Lip] = FName(*costumeInfo->GetStringField("lip"));

		TSharedPtr<FJsonObject> eyeInfo = avatar->GetObjectField(TEXT("eyeInfo"));
		info.AvatarInfo.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::EyeColor] = FCString::Atof(*eyeInfo->GetStringField("eyeColor"));
		info.AvatarInfo.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::IrisBrightness] = FCString::Atof(*eyeInfo->GetStringField("irisBrightness"));
		info.AvatarInfo.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::PupilScale] = FCString::Atof(*eyeInfo->GetStringField("pupilScale"));
		info.AvatarInfo.EyeMaterialScalarParameter[EEyeMaterialScalarParameter::ScleraBrightness] = FCString::Atof(*eyeInfo->GetStringField("scleraBrightness"));

		TSharedPtr<FJsonObject> morphInfo = avatar->GetObjectField(TEXT("morphInfo"));
		int i = 0;
		for (auto MorphTarget : info.AvatarInfo.MorphTargetValue)
		{
			FString str = EnumToString(TEXT("EMorphTarget"), i);
			str[0] = std::tolower(str[0]);
			str = str.Replace(TEXT(" "), TEXT(""), ESearchCase::Type::IgnoreCase);
			info.AvatarInfo.MorphTargetValue[i++].Value = FCString::Atof(*morphInfo->GetStringField(str));
		}

		Result.Add(info);
	}
}

URequestTargetsSocialInfo* URequestTargetsSocialInfo::RequestTargetsSocialInfo(const TArray<int32>& TargetIDs)
{
	URequestTargetsSocialInfo* BPNode = NewObject<URequestTargetsSocialInfo>();
	BPNode->TargetIDs = TargetIDs;
	return BPNode;
}

URequestTargetsSocialInfo* URequestTargetsSocialInfo::GetMySocialInfo()
{
	URequestTargetsSocialInfo* BPNode = NewObject<URequestTargetsSocialInfo>();
	BPNode->TargetIDs.Add(URMProtocolFunctionLibrary::GetUserSeq());
	return BPNode;
}

void URequestTargetsSocialInfo::Activate()
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("user/info"), TEXT("POST"));
	
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	TArray <TSharedPtr<FJsonValue>> ids;
	for (int32 id : TargetIDs)
	{
		ids.Add(MakeShared<FJsonValueString>(FString::FromInt(id)));
	}
	_payloadJson->SetStringField("page", "1");
	_payloadJson->SetStringField("showCnt", FString::FromInt(TargetIDs.Num()));
	_payloadJson->SetArrayField("keywordList", ids);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URequestTargetsSocialInfo::OnResponse);
		UE_LOG(LogFriend, Log, TEXT("RequestTargetsSocialInfo : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogFriend, Warning, TEXT("RequestTargetsSocialInfo - cannot process request"));
		Finished.Broadcast(0, TArray<FSocialInfo>());
		SetReadyToDestroy();
	}
}

void URequestTargetsSocialInfo::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	int32 ErrorCode;
	FString ErrorMessage;

	if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	{
		UE_LOG(LogFriend, Warning, TEXT("%s"), *ErrorMessage);
		Finished.Broadcast(ErrorCode, TArray<FSocialInfo>());
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogFriend, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}

		TArray<FSocialInfo> Result;
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{	
			TSharedPtr<FJsonObject> data = res->GetObjectField("data");
		
			ToSocialInfo(data->GetArrayField("list"), Result);
			Finished.Broadcast(resultCode, Result);
		}
		else
		{
			Finished.Broadcast(resultCode, Result);
		}
	}
	SetReadyToDestroy();
}

// --------------------------------------------------------------------------------------------------------------------------------

URequestFriendsSocialInfo* URequestFriendsSocialInfo::RequestFriendsSocialInfo(int32 Page, int32 ShowCnt, bool IsFollowing)
{
	URequestFriendsSocialInfo* BPNode = NewObject<URequestFriendsSocialInfo>();
	BPNode->Page = Page;
	BPNode->ShowCnt = ShowCnt;
	BPNode->IsFollowing = IsFollowing;
	return BPNode;
}

void URequestFriendsSocialInfo::Activate()
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("friends/list"), TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("page", FString::FromInt(Page));
	_payloadJson->SetStringField("showCnt", FString::FromInt(ShowCnt));
	_payloadJson->SetStringField("keyword", IsFollowing ? "" : "follower");

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URequestFriendsSocialInfo::OnResponse);
		UE_LOG(LogFriend, Log, TEXT("RequestFriendsSocialInfo : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogFriend, Warning, TEXT("RequestFriendsSocialInfo - cannot process request"));
		Finished.Broadcast(0, 0, 0, 0, TArray<FSocialInfo>());
		SetReadyToDestroy();
	}
}

void URequestFriendsSocialInfo::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	int32 ErrorCode;
	FString ErrorMessage;

	if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	{
		UE_LOG(LogFriend, Warning, TEXT("%s"), *ErrorMessage);
		Finished.Broadcast(ErrorCode, 0, 0, 0, TArray<FSocialInfo>());
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogFriend, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}

		TArray<FSocialInfo> Result;
		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			TSharedPtr<FJsonObject> data = res->GetObjectField("data");

			int32 FollowingCount = 0;
			int32 FollowerCount = 0;
			ToFollowCount(data->GetObjectField("myInfo"), FollowingCount, FollowerCount);
			ToSocialInfo(data->GetArrayField("list"), Result);
			Finished.Broadcast(resultCode, data->GetIntegerField("totalPage"), FollowingCount, FollowerCount, Result);
		}
		else
		{
			Finished.Broadcast(resultCode, 0, 0, 0, Result);
		}
	}
	SetReadyToDestroy();
}

// --------------------------------------------------------------------------------------------------------------------------------

USearchFriends* USearchFriends::SearchFriends(const FString& Keyword)
{
	USearchFriends* BPNode = NewObject<USearchFriends>();
	BPNode->Keyword = Keyword;
	return BPNode;
}

USearchFriends* USearchFriends::RequestRecommendedFriends()
{
	USearchFriends* BPNode = NewObject<USearchFriends>();
	return BPNode;
}

void USearchFriends::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/friends/search"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));
	
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("page", "1");
	_payloadJson->SetStringField("showCnt", "20");
	if (!Keyword.IsEmpty())
		_payloadJson->SetStringField("keyword", Keyword);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &USearchFriends::OnResponse);
		UE_LOG(LogFriend, Log, TEXT("SearchFriends : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogFriend, Warning, TEXT("SearchFriends - cannot process request"));
		Finished.Broadcast(0, TArray<FSocialInfo>());
		SetReadyToDestroy();
	}
}

void USearchFriends::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	//int32 ErrorCode;
	FString ErrorMessage;

	if(Response == nullptr)
	{
		SetReadyToDestroy();
		Finished.Broadcast(0, TArray<FSocialInfo>());
		return;
	}

	if(!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), TArray<FSocialInfo>());
		SetReadyToDestroy();
		return;
	}

	// if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	// {
	// 	UE_LOG(LogFriend, Warning, TEXT("%s"), *ErrorMessage);
	// 	Finished.Broadcast(ErrorCode, TArray<FSocialInfo>());
	// 	SetReadyToDestroy();
	// 	return;
	// }

	{
		UE_LOG(LogFriend, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}

			TArray<FSocialInfo> Result;
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				TSharedPtr<FJsonObject> data = res->GetObjectField("data");

				ToSocialInfo(data->GetArrayField("list"), Result);
				Finished.Broadcast(resultCode, Result);
			}
			else
			{
				Finished.Broadcast(resultCode, Result);
			}
		}
	}
	
	SetReadyToDestroy();
}


// --------------------------------------------------------------------------------------------------------------------------------

UFollowFriend* UFollowFriend::Follow(int32 UserSeq)
{
	UFollowFriend* BPNode = NewObject<UFollowFriend>();
	BPNode->UserSeq = UserSeq;
	BPNode->IsFollow = true;
	return BPNode;
}

UFollowFriend* UFollowFriend::UnFollow(int32 UserSeq)
{
	UFollowFriend* BPNode = NewObject<UFollowFriend>();
	BPNode->UserSeq = UserSeq;
	BPNode->IsFollow = false;
	return BPNode;
}

void UFollowFriend::Activate()
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("friends/follow"), IsFollow ? TEXT("POST") : TEXT("DELETE"));
	
	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("userSeq", FString::FromInt(UserSeq));
	
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UFollowFriend::OnResponse);
		UE_LOG(LogFriend, Log, TEXT("UFollowFriend : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogFriend, Warning, TEXT("UFollowFriend - cannot process request"));
		Finished.Broadcast(0, EFollowState::ENone);
		SetReadyToDestroy();
	}
}

void UFollowFriend::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	int32 ErrorCode;
	FString ErrorMessage;

	if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	{
		UE_LOG(LogFriend, Warning, TEXT("%s"), *ErrorMessage);
		Finished.Broadcast(ErrorCode, EFollowState::ENone);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogFriend, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}

		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			TSharedPtr<FJsonObject> data = res->GetObjectField("data");
			Finished.Broadcast(resultCode, (EFollowState)data->GetIntegerField("follow"));
		}
		else
		{
			Finished.Broadcast(resultCode, EFollowState::ENone);
		}
	}
	SetReadyToDestroy();
}


// --------------------------------------------------------------------------------------------------------------------------------

URequestTargetPosition* URequestTargetPosition::RequestTargetPosition(int32 TargetUserSeq)
{
	URequestTargetPosition* BPNode = NewObject<URequestTargetPosition>();
	BPNode->TargetUserSeq = TargetUserSeq;
	return BPNode;
}

void URequestTargetPosition::Activate()
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("friends/position"), TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("userSeq", FString::FromInt(TargetUserSeq));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URequestTargetPosition::OnResponse);
		UE_LOG(LogFriend, Log, TEXT("URequestTargetPosition : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogFriend, Warning, TEXT("URequestTargetPosition - cannot process request"));
		Finished.Broadcast(0, FSocialInfo());
		SetReadyToDestroy();
	}
}

void URequestTargetPosition::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	int32 ErrorCode;
	FString ErrorMessage;
	FSocialInfo SocialInfo;

	if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	{
		UE_LOG(LogFriend, Warning, TEXT("%s"), *ErrorMessage);
		Finished.Broadcast(ErrorCode, SocialInfo);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogFriend, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}

		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			TSharedPtr<FJsonObject> data = res->GetObjectField("data");

			if (data->HasField("room"))
				SocialInfo.LevelName = data->GetStringField("room");
			if (data->HasField("channel"))
				SocialInfo.ChannelName = data->GetStringField("channel");
			FDateTime::Parse(data->GetStringField("lastLoginDate"), SocialInfo.LastLoginTime);

			Finished.Broadcast(resultCode, SocialInfo);
		}
		else
		{
			Finished.Broadcast(resultCode, SocialInfo);
		}
	}
	SetReadyToDestroy();
}

URequestTargetInvite* URequestTargetInvite::RequestTargetInvite(int32 TargetUserSeq)
{
	URequestTargetInvite* BPNode = NewObject<URequestTargetInvite>();
	BPNode->TargetUserSeq = TargetUserSeq;
	return BPNode;
}

void URequestTargetInvite::Activate()
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("friends/invite"), TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("userSeq", FString::FromInt(TargetUserSeq));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URequestTargetInvite::OnResponse);
		UE_LOG(LogFriend, Log, TEXT("URequestTargetPosition : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogFriend, Warning, TEXT("URequestTargetPosition - cannot process request"));
		Finished.Broadcast(0, FSocialInfo());
		SetReadyToDestroy();
	}
}

void URequestTargetInvite::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	int32 ErrorCode;
	FString ErrorMessage;
	FSocialInfo SocialInfo;

	if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	{
		UE_LOG(LogFriend, Warning, TEXT("%s"), *ErrorMessage);
		Finished.Broadcast(ErrorCode, SocialInfo);
		SetReadyToDestroy();
		return;
	}

	UE_LOG(LogFriend, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			SetReadyToDestroy();
			return;
		}

		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			TSharedPtr<FJsonObject> data = res->GetObjectField("data");

			if (data->HasField("room"))
				SocialInfo.LevelName = data->GetStringField("room");
			if (data->HasField("channel"))
				SocialInfo.ChannelName = data->GetStringField("channel");
			FDateTime::Parse(data->GetStringField("lastLoginDate"), SocialInfo.LastLoginTime);

			Finished.Broadcast(resultCode, SocialInfo);
		}
		else
		{
			Finished.Broadcast(resultCode, SocialInfo);
		}
	}
	SetReadyToDestroy();
}

// --------------------------------------------------------------------------------------------------------------------------------

URMFriendObject::URMFriendObject()
	: Super()
{
	
}

void URMFriendObject::Init()
{
	if (LoadChatData())
		bIsInit = true;
}

void URMFriendObject::ResetObject()
{
	if (bIsInit)
	{
		ClearChatData();
		AllMessages.Empty(0);
		OnFinishedRequestNewChatLists.Clear();
		CurrentFriendSeq = -1;
		SaveFileName.Empty(0);
		bIsInit = false;
	}
}


bool URMFriendObject::SaveChatData()
{
	if (URMProtocolFunctionLibrary::GetUserSeq())
		SaveFileName = SaveFileName.IsEmpty() ? FString::Printf(TEXT("%s_FriendChatData"), *FString::FromInt(URMProtocolFunctionLibrary::GetUserSeq()))
											: SaveFileName;
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("You must create ChatSaveData after GetUserAccountData"));
		return false;
	}
		
	auto saveData = Cast<UUserChatSaveGame>(UGameplayStatics::CreateSaveGameObject(UUserChatSaveGame::StaticClass()));
	saveData->AllMessages = this->AllMessages;
	bool bSucceeded =  UGameplayStatics::SaveGameToSlot(saveData, SaveFileName, 0);
	saveData->ConditionalBeginDestroy();
	return bSucceeded;
}

bool URMFriendObject::LoadChatData()
{
	if (URMProtocolFunctionLibrary::GetUserSeq())
		SaveFileName = SaveFileName.IsEmpty() ? FString::Printf(TEXT("%s_FriendChatData"), *FString::FromInt(URMProtocolFunctionLibrary::GetUserSeq()))
										: SaveFileName;
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("You must create ChatSaveData after GetUserAccountData"));
		return false;
	}
	
	UUserChatSaveGame* saveData = Cast<UUserChatSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveFileName, 0));
	if (saveData == nullptr)
	{
		if (SaveChatData())
			bIsInit = true;
		return false;
	}
	else
	{
		AllMessages = saveData->AllMessages;
		saveData->ConditionalBeginDestroy();
		return true;
	}
}

void URMFriendObject::ClearChatData()
{
	if (URMProtocolFunctionLibrary::GetUserSeq())
		SaveFileName = SaveFileName.IsEmpty() ? FString::Printf(TEXT("%s_FriendChatData"), *FString::FromInt(URMProtocolFunctionLibrary::GetUserSeq())) : SaveFileName;
	
	
	UGameplayStatics::DeleteGameInSlot(SaveFileName, 0);
}

TArray<FFriendChatMessage> URMFriendObject::GetChatMessage(int32 FriendUserSeq)
{
	if (bIsInit && AllMessages.Contains(FriendUserSeq))
		return AllMessages.Find(FriendUserSeq)->msgArr;

	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Can't Find Messages"));
	}

	TArray<FFriendChatMessage> R;
	R.Init(FFriendChatMessage(), 0);
	return R;
}

TArray<FFriendChatMessage> URMFriendObject::GetChatMessageForRange(int32 FriendUserSeq, int32 Count, int32& returnStartIndex, int32 LastIndex)
{
	/* Start Index를 확인하는게 맞을까? */
	TArray<FFriendChatMessage> outArr = { };
	outArr.Init(FFriendChatMessage(), 0);
	returnStartIndex = -1;
	if (bIsInit && AllMessages.Contains(FriendUserSeq))
	{
		auto msgArr = AllMessages.Find(FriendUserSeq)->msgArr;
		
		// 주어진 LastIndex가 범위 내에 있는지?
		if (LastIndex < 0 || LastIndex >= msgArr.Num())
			LastIndex = msgArr.Num() - 1;
		
		returnStartIndex = (LastIndex - Count + 1) >= 0 ? LastIndex - Count + 1 : 0;
		int arrSize = LastIndex - returnStartIndex + 1;
		
		if (arrSize <= 0)
			returnStartIndex = -1;
		else
		{
			outArr.Empty(0);
			for (int i = returnStartIndex; i < returnStartIndex + arrSize; i++)
				outArr.Add(msgArr[i]);
			// TO DO: Widget에서 Reverse Scrolling 처리가 가능한지 확인. (처리 가능)
		}
	}
	else if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Can't Find Messages"));
	
	return outArr;
}

void URMFriendObject::TryRequestNewChatList(int32 FriendUserSeq, FString Text, const FFinishedRequestNewChatList& onCompleteRequest)
{
	if (OnFinishedRequestNewChatLists.IsBound())
		OnFinishedRequestNewChatLists.Clear();

	// if not init chat object, return.
	if (FriendUserSeq < 0 || !bIsInit)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f , FColor::Red, TEXT("must input correct or Object init first"));
		return;
	}

	// save param for response function.
	CurrentFriendSeq = FriendUserSeq;
	OnFinishedRequestNewChatLists = onCompleteRequest;

	// Start HttpRequest.
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("friends/chat"), TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("to", FString::FromInt(CurrentFriendSeq));
	_payloadJson->SetStringField("text", Text);
	
	int32 lastChatID = 0;
	if (AllMessages.Contains(CurrentFriendSeq) && AllMessages.Find(CurrentFriendSeq)->msgArr.Num())
		lastChatID = AllMessages.Find(CurrentFriendSeq)->msgArr.Last().ChatID;
	_payloadJson->SetStringField("prevChatId", FString::FromInt(lastChatID));

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);
	
	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URMFriendObject::OnResponseRequestHttp);
		UE_LOG(LogFriend, Log, TEXT("URequestFriendsMessage : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogFriend, Warning, TEXT("RequestFriendsMessage - cannot process request"));
		ExecuteHttpRequestFinished(false, 0, TArray<FFriendChatMessage>());
	}
	
}

void URMFriendObject::OnResponseRequestHttp(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	int32 ErrorCode = 0;
	FString ErrorMessage = TEXT("");
	TArray<FFriendChatMessage> Messages;

	if (!URMProtocolFunctionLibrary::CheckResponse(Response, WasSuccessful, GetClass(), ErrorCode, ErrorMessage))
	{
		UE_LOG(LogFriend, Warning, TEXT("%s"), *ErrorMessage);
		ExecuteHttpRequestFinished(false, ErrorCode, Messages);
		return;
	}

	UE_LOG(LogFriend, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

	TSharedPtr<FJsonObject> res;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, res))
	{
		if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
		{
			ExecuteHttpRequestFinished(false, ErrorCode, Messages);
			return;
		}

		int resultCode = res->GetIntegerField("resultCode");
		if (resultCode == 200)
		{
			FFriendChatMessage lastMessage = (GetChatMessage(CurrentFriendSeq).Num() > 0)
			? GetChatMessage(CurrentFriendSeq).Last() : FFriendChatMessage();
			
			TSharedPtr<FJsonObject> data = res->GetObjectField("data");
			TArray<TSharedPtr<FJsonValue>> chatList = data->GetArrayField("chatList");

			// Check my sending message.
			if (data->HasField("chat"))
			{
				TSharedPtr<FJsonObject> myChat = data->GetObjectField("chat");
				if (myChat->HasField("chatId"))
				{
					FFriendChatMessage msg;
					msg.Text = myChat->GetStringField("text");
					msg.To = myChat->GetIntegerField("to");
					msg.From = myChat->GetIntegerField("from");
					msg.ChatID = myChat->GetIntegerField("chatId");
					msg.RegDate = FDateTime::UtcNow();
					
					Messages.Add(msg);
				}
			}

			// check recieved chat list.
			for (TSharedPtr<FJsonValue> chatValue : chatList)
			{
				FFriendChatMessage msg;
				
				TSharedPtr<FJsonObject> chatObj = chatValue->AsObject();

				msg.To = (chatObj->HasField("to")) ? chatObj->GetIntegerField("to") : 0;
				msg.From = (chatObj->HasField("from")) ? chatObj->GetIntegerField("from") : 0;
				msg.Text = (chatObj->HasField("text")) ? chatObj->GetStringField("text") : TEXT("");
				msg.ChatID = (chatObj->HasField("chatId")) ? chatObj->GetIntegerField("chatId") : -1;
				
				if (chatObj->HasField("regdate"))
					FDateTime::Parse(chatObj->GetStringField("regdate"), msg.RegDate);
				
				Messages.Add(msg);
			}
			
			if(Messages.Num() > 0)
			{
				// Sort New Messages.
				Messages.StableSort([](const FFriendChatMessage& A, const FFriendChatMessage& B)
				{	// Compare Start
					return A.ChatID < B.ChatID;
				});	// Compare End

				// Check DateInfo.
				for (int i = 0; i < Messages.Num(); i++)
				{
					Messages[i].bVisibleDayInfo = GetVisibilityDayInfo(lastMessage.RegDate, Messages[i].RegDate);
					lastMessage = Messages[i];
				}

				// Add Messages in Object.
				if (AllMessages.Contains(CurrentFriendSeq))
					AllMessages.Find(CurrentFriendSeq)->msgArr.Append(Messages);
				else
				{
					FChatArray chatArr;
					chatArr.msgArr = Messages;
					AllMessages.Add(CurrentFriendSeq, chatArr);
				}
			}

			// Save Messages.
			SaveChatData();

			// Broadcast Delegate.
			ExecuteHttpRequestFinished(true, resultCode, Messages);
			return;
		}
		ExecuteHttpRequestFinished(false, resultCode, Messages);
	}
}

void URMFriendObject::ExecuteHttpRequestFinished(bool bSucceeded, int32 ResultCode, TArray<FFriendChatMessage> NewMessages)
{
	// Execute in gameThread. for umg logic.
	if (OnFinishedRequestNewChatLists.IsBound())
	{
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(FSimpleDelegateGraphTask::FDelegate::CreateLambda([this, bSucceeded, ResultCode, NewMessages]()
		{	// Start Lambda
			if (OnFinishedRequestNewChatLists.IsBound())
				OnFinishedRequestNewChatLists.Execute(bSucceeded, ResultCode, NewMessages);
			OnFinishedRequestNewChatLists.Clear();
		})	// End Lambda
		, TStatId(), nullptr, ENamedThreads::GameThread);	
	}
}

inline bool URMFriendObject::GetVisibilityDayInfo(FDateTime lastMessageDateTime, FDateTime currentMessageDateTime)
{
	return (currentMessageDateTime - lastMessageDateTime).GetDays() > 0;
}

URequestFCMSign* URequestFCMSign::RequestFCMSign(FString FCMToken, FString Lang)
{
	URequestFCMSign* BPNode = NewObject<URequestFCMSign>();
	BPNode->FCMToken = FCMToken;
	BPNode->Lang = Lang;
	return BPNode;
}

void URequestFCMSign::Activate()
{
	Super::Activate();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/fcm/sign"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	_payloadJson->SetStringField("token", FCMToken);
	_payloadJson->SetStringField("lang", Lang);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &URequestFCMSign::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("token : %s"), *_payload);
	}
	else
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("UChangeNicknameAPI - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"));
		SetReadyToDestroy();
	}
}

void URequestFCMSign::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	// iPhone의 경우 네트워크 연결이 끊어지면 Response == nullptr 현상 발생
	if (Response == nullptr)
	{
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."));
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		Finished.Broadcast(Response->GetResponseCode(), false, msg);
		SetReadyToDestroy();
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
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				Finished.Broadcast(resultCode, true, TEXT(""));
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"));
			}
		}
	}
	SetReadyToDestroy();
}


UPostFriendRank* UPostFriendRank::PostFriendRank(const FString SortType, const FString keyword)
{
	UPostFriendRank* BPNode = NewObject<UPostFriendRank>();
	BPNode->SortType = SortType;
	BPNode->keyword = keyword;
	return BPNode;
	
}

void UPostFriendRank::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/friends/follow-rank"), *URMProtocolFunctionLibrary::GetBaseUrl()));
	URMProtocolFunctionLibrary::SetDefaultHeader(HttpRequest);
	HttpRequest->SetHeader(TEXT("authorization"), *URMProtocolFunctionLibrary::GetAccessToken());
	HttpRequest->SetVerb(TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

	_payloadJson->SetStringField("SortType", SortType);
	_payloadJson->SetStringField("keyword", keyword);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPostFriendRank::OnResponse);
		UE_LOG(LogRMProtocol, Log, TEXT("UAttendEvent : %s"), *HttpRequest->GetURL());
	}
	else
	{
		TArray<FFriendFollowRankElem> Result;
		UE_LOG(LogRMProtocol, Warning, TEXT("UAttendEvent - cannot process request"));
		Finished.Broadcast(0, false, TEXT("cannot process request"),Result);
		SetReadyToDestroy();
	}
}

void UPostFriendRank::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (Response==nullptr)
	{
		TArray<FFriendFollowRankElem> Result;
		Finished.Broadcast(0, false, TEXT("The Internet connection appears to be offline."), Result);
		SetReadyToDestroy();
		return;
	}

	if (!WasSuccessful || Response->GetResponseCode() != 200)
	{
		FString msg = URMProtocolFunctionLibrary::GetProtocolErrorMessage(Response, GetClass());
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *msg);
		TArray<FFriendFollowRankElem> ResultReturn;
		Finished.Broadcast(Response->GetResponseCode(), false, msg, ResultReturn);
		SetReadyToDestroy();
		return;
	}

	
	//if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogRMProtocol, Warning, TEXT("OnResponse : %s"), *Response->GetContentAsString());

		TArray<FFriendFollowRankElem> RankArray;
		RankArray.Empty();

		TSharedPtr<FJsonObject> res;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(reader, res))
		{
			if (URMProtocolFunctionLibrary::CheckSubErrorCode(res))
			{
				SetReadyToDestroy();
				return;
			}
			int resultCode = res->GetIntegerField("resultCode");
			if (resultCode == 200)
			{
				auto const returnObject = res->GetObjectField("data");
				for (auto Elem : returnObject->GetArrayField("followRanks"))
				{
					const auto Info = Elem->AsObject();
					FFriendFollowRankElem ArrayElem;
					ArrayElem.userSeq = Info->GetStringField("userSeq");
					ArrayElem.nickname = Info->GetStringField("nickname");
					ArrayElem.club = Info->GetNumberField("club");
					ArrayElem.countrySP = Info->GetStringField("countrySP");
					ArrayElem.followCnt = Info->GetNumberField("follwCnt");

					RankArray.Add(ArrayElem);
				}

				Finished.Broadcast(resultCode, true, TEXT(""), RankArray);
			}
			else
			{
				Finished.Broadcast(resultCode, false, res->GetStringField("resultMessage"), RankArray);
			}
		}
	}
	SetReadyToDestroy();
}



// --------------------------------------------------------------------------------------------------------------------------------
