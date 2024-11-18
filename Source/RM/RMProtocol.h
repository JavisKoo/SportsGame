// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RMStruct.h"
#include "RMEventSubsystem.h"
#include "RMProtocol.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FServerInfoSettingFinishedPin, int32, ResultCode, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UServerInfoSetting : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UServerInfoSetting() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FServerInfoSettingFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UServerInfoSetting* SetServerInfo();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	static FString FirstUrl;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FLoginFinishedPin, FString, UserID, bool, IsNewbie, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API ULoginAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	ULoginAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FLoginFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static ULoginAPI* Login(FString FirebaseToken, FString GoogleToken);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString _FirebaseToken;
	FString _GoogleToken;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FETC_DataFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, FString, Data);

UCLASS()
class RM_API UETC_DataAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UETC_DataAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FETC_DataFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UETC_DataAPI* ETC_Data();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
	
};

#pragma region PARSE
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FLoginWithParseFinishedPin, FString, UserID, FString, SessionToken, bool, IsNewbie, int32, ResultCode, bool, Success, FString, ErrMessage);
UCLASS()
class RM_API ULoginWithParseAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	ULoginWithParseAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FLoginWithParseFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static ULoginWithParseAPI* LoginWithParse(ELoginWithParseType LoginWithParseType, FString ID_Token, FString UserID, FString SessionToken);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	ELoginWithParseType LoginWithParseType;
	FString ID_Token;
	FString UserID;
	FString SessionToken;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCreateAccountFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UAccountConnectionWithParse : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UAccountConnectionWithParse() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FCreateAccountFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Parse")
		static UAccountConnectionWithParse* AccountConnectionWithParse(ELoginWithParseType parseType, FString ID_Token, FString UserID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	ELoginWithParseType LoginWithParseType;
	FString ID_Token;
	FString UserID;
};
#pragma endregion


/**
 *
 */
UCLASS()
class RM_API UCreateAccountAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UCreateAccountAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FCreateAccountFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UCreateAccountAPI* CreateAccount(const FString& Club, const FString& Nickname, const FName& Flag, const FAvatarInfo2& AvatarInfo);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString club;
	FString Nickname;
	FName Flag;
	FAvatarInfo2 AvatarInfo;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGetUserInfoFinishedPin, FUserInfo, UserInfo, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UGetUserInfoAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetUserInfoAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FGetUserInfoFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UGetUserInfoAPI* GetUserInfo();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FChangeNicknameFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UChangeNicknameAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UChangeNicknameAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FChangeNicknameFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UChangeNicknameAPI* ChangeNickname(const FString& Nickname);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString Nickname;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateUserStateFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UUpdateUserStateAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UUpdateUserStateAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FUpdateUserStateFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UUpdateUserStateAPI* UpdateUserState(const int32& UserState);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
private:
	int32 UserState;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateTeamSelectFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UUpdateTeamSelectAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UUpdateTeamSelectAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FUpdateTeamSelectFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UUpdateTeamSelectAPI* UpdateTeamSelect(const FName& teamSelect);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
private:
	FName TeamSelect;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateAvatarFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UUpdateAvatarInfoAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UUpdateAvatarInfoAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FUpdateAvatarFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UUpdateAvatarInfoAPI* UpdateAvatarInfo(const FAvatarInfo2& AvatarInfo);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FAvatarInfo2 AvatarInfo;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBuyCostumeFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UBuyCostumeAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UBuyCostumeAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FBuyCostumeFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UBuyCostumeAPI* BuyCostume(const FName& CostumeKey);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString CostumeKey;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBuyContentPackFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UBuyContentPackAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UBuyContentPackAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FBuyContentPackFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UBuyContentPackAPI* BuyContentPack(const FName& CostumeKey);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString CostumeKey;
};


// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPurchaseGoogleFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);
//
// /**
//  *
//  */
// UCLASS()
// class RM_API UPurchaseGoogleAPI : public UBlueprintAsyncActionBase
// {
// 	GENERATED_BODY()
//
// public:
// 	UPurchaseGoogleAPI() {};
//
// 	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
// 	FPurchaseGoogleFinishedPin Finished;
//
// private:
// 	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
// 	static UPurchaseGoogleAPI* PurchaseGoogle(const FString& receiptData);
//
// 	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
// 		static UPurchaseGoogleAPI* GiftGoogle(const FString& receiptData, int32 TargetSeq);
//
// 	virtual void Activate() override;
// 	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
//
// private:
// 	FString receiptData;
// 	int32 TargetSeq;
// };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPurchaseAppleFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
*
*/
UCLASS()
class RM_API UPurchaseAppleAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	public:
	UPurchaseAppleAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FPurchaseAppleFinishedPin Finished;

	private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UPurchaseAppleAPI* PurchaseApple(const FString& ReceiptData);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UPurchaseAppleAPI* GiftApple(const FString& ReceiptData, int32 TargetSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	private:
	FString ReceiptData;
	int32 TargetSeq;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPurchaseEditorFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UPurchaseEditorAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPurchaseEditorAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FPurchaseEditorFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UPurchaseEditorAPI* PurchaseEditor(const FString& ProductId);
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UPurchaseEditorAPI* GiftEditor(const FString& ProductId, int32 TargetSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	FString ProductId;
	int32 TargetSeq;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAddCoinFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UAddCoinAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAddCoinAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FAddCoinFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UAddCoinAPI* AddCoin(int32 Coin);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 Coin;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDeleteAccountFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UDeleteAccountAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UDeleteAccountAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FDeleteAccountFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UDeleteAccountAPI* DeleteAccount();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSetCountryFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API USetCountryAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	USetCountryAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FSetCountryFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static USetCountryAPI* SetCountry(const FName& Country);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FName Country;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSetFlagFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API USetFlagAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	USetFlagAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FSetFlagFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static USetFlagAPI* SetFlag(const FName& Flag);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FName Flag;
};

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCreatePartyRoomFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);
//
///**
// *
// */
//UCLASS()
//class RM_API UCreatePartyRoomAPI : public UBlueprintAsyncActionBase
//{
//	GENERATED_BODY()
//
//public:
//	UCreatePartyRoomAPI() {};
//
//	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
//		FCreatePartyRoomFinishedPin Finished;
//
//private:
//	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
//		static UCreatePartyRoomAPI* CreatePartyRoom(int32 ItemID);
//
//	virtual void Activate() override;
//	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
//
//private:
//	int32 ItemID;
//};



//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDownloadConfigFileFinishedPin, int32, ResultCode, FString, ErrMessage);
//
///**
// *
// */
//UCLASS()
//class RM_API UDownloadConfigFile : public UBlueprintAsyncActionBase
//{
//	GENERATED_BODY()
//
//public:
//	UDownloadConfigFile() {};
//
//	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
//		FDownloadConfigFileFinishedPin Finished;
//
//private:
//	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
//		static UDownloadConfigFile* DownloadConfigFile();
//
//	virtual void Activate() override;
//	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
//};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetManifestListFinishedPin, bool, Success, FString, CloudURL, const TArray<FString>&, ManifestURLs, const TArray<FString>&, InstallDir);

/**
 *
 */
UCLASS()
class RM_API UGetManifestList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetManifestList() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetManifestListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetManifestList* GetManifestList();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};





#pragma region PurchaseHmsAPI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPurchaseHmsFinishedPin, int32, ResultCode, bool, WasSuccessful, FString, ErrMessage);
UCLASS()
class RM_API UPurchaseHmsAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	public:
	UPurchaseHmsAPI(){}

	UPROPERTY(BlueprintAssignable, Category="RMProtocol")
	FPurchaseHmsFinishedPin Finished;

	private:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"), Category="Protocol")
	static UPurchaseHmsAPI* PurchaseHms(const FString& PurchaseToken, const FString& ProductId);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Protocol")
		static UPurchaseHmsAPI* GiftHms(const FString& PurchaseToken, const FString& ProductId, int32 TargetSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	FString PurchaseToken;
	FString ProductId;
	int32 TargetSeq;
};
#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTestHttpFinishedPin, bool, success);

/**
 *
 */
UCLASS()
class RM_API UTestHttp : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UTestHttp() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FTestHttpFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UTestHttp* TestHttp();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

#pragma region AttendanceRewardAPI

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAttendanceRewardFinishedPin, bool, Success, FUserInfo, UserInfo, int32, ResultCode,  FString, ResultMessage);

UCLASS()
class RM_API UAttendanceRewardAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAttendanceRewardAPI(){};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FAttendanceRewardFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UAttendanceRewardAPI* AttendanceReward(const EQuestType& QuestType, const int32& QuestSequence);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	EQuestType QuestType;
	int32 QuestSequence;
};

#pragma endregion



#pragma region VoiceCoin

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBuyVoiceCoinFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UBuyVoiceCoinAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UBuyVoiceCoinAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FBuyVoiceCoinFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UBuyVoiceCoinAPI* BuyVoiceCoin(const FName& ItemID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString ItemID;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUseVoiceCoinFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UUseVoiceCoinAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UUseVoiceCoinAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FUseVoiceCoinFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UUseVoiceCoinAPI* UseVoiceCoin();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

#pragma endregion


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultipleAccessFinishedPin, bool, success);

/**
 *
 */
UCLASS()
class RM_API UMultipleAccess : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UMultipleAccess() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FMultipleAccessFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UMultipleAccess* CheckMultipleAccess(const FString& LevelName, const FString& RoomName);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString LevelName;
	FString RoomName;
};




DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetPartyRoomListFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, const TArray<FPartyRoom>&, RoomList);

/**
 *
 */
UCLASS()
class RM_API UGetPartyRoomList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetPartyRoomList() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetPartyRoomListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetPartyRoomList* GetPartyRoomList(EPartySorting Sorting, int32 Page, int32 Count, FString KeyWord);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	EPartySorting Sorting;
	int32 Page;
	int32 Count;
	FString KeyWord;
};




DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCreatePartyRoomFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, FPartyRoom, RoomInfo);

/**
 *
 */
UCLASS()
class RM_API UCreatePartyRoom : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UCreatePartyRoom() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FCreatePartyRoomFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UCreatePartyRoom* CreatePartyRoom(FString RoomName, FName RowName, bool IsShare, FString PW);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString RoomName;
	FName RowName;
	bool IsShare;
	FString PW;
};


#pragma region EMAIL
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetEmailListDel, bool, bSuccess, int32, ResultCode, FString, ResultMessage, const TArray<FEmailData>&, EmailInfo);
UCLASS()
class RM_API UGetEmailList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetEmailList() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetEmailListDel Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetEmailList* GetEmailList(int lastEmailSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int userEmailSeq = 0;

};

UCLASS()
class RM_API UGetEmailAttachment : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UGetEmailAttachment() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FUseVoiceCoinFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetEmailAttachment* GetAttachment(int userEmailSeq, int AttachmentSeq, EItemType itemType, int itemID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int userEmailSeq;
	int attachmentSeq;
	EItemType itemType;
	int itemID;
};

#pragma endregion



DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FIsExistRoomFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, FPartyRoom, RoomInfo);

/**
 *
 */
UCLASS()
class RM_API UIsExistRoom : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UIsExistRoom() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FIsExistRoomFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UIsExistRoom* IsExistRoom(FString RoomTitle, int32 RoomID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 RoomID;
	FString RoomTitle;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FLikePartyRoomFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API ULikePartyRoom : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	ULikePartyRoom() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FLikePartyRoomFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static ULikePartyRoom* LikePartyRoom(int32 RoomID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 RoomID;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetRecommendedUsersFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, const TArray<FSearchedUser>&, Users);

/**
 *
 */
UCLASS()
class RM_API UGetRecommendedUsers : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetRecommendedUsers() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetRecommendedUsersFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetRecommendedUsers* GetRecommendedUsers();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FFindUserFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, const TArray<FSearchedUser>&, Users);

/**
 *
 */
UCLASS()
class RM_API UFindUser : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFindUser() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FFindUserFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UFindUser* FindUser(FString Nickname);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString Nickname;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInviteUserFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UInviteUser : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UInviteUser() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FInviteUserFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UInviteUser* InviteUser(int32 UserSeq, int32 RoomID);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 UserSeq;
	int32 RoomID;
};

#pragma region MINIGAME_INFO
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLoadMiniGameInfoDel, int32, ResultCode, bool, Success, FString, ErrMessage, FMiniGameInfo, MiniGameInfo);

UCLASS()
class RM_API ULoadMiniGameInfo : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	ULoadMiniGameInfo() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FLoadMiniGameInfoDel Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static ULoadMiniGameInfo* LoadMiniGameInfo(int32 page, int32 loadCountInOnePage, EGameRankType rankType, EGameType gameSeq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 requestPage;
	int32 loadCount;
	EGameRankType rankType;
	EGameType gameType;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSaveMiniGameInfoDel, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API USaveMiniGameInfo : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	USaveMiniGameInfo() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FSaveMiniGameInfoDel Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static USaveMiniGameInfo* SaveMiniGameInfo(EGameType gameSeq, int32 score);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 gameSeq;
	int32 score;

};
#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReportUserFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API UReportUser : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UReportUser() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FReportUserFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UReportUser* ReportUser(FString Nickname, FString Desc);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString Nickname;
	FString Desc;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetRewardTableDelegate, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FRewardTable>&, rewardTable);

UCLASS()
class RM_API UGetRewardTable : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UGetRewardTable() {};

	UPROPERTY(BlueprintAssignable)
		FGetRewardTableDelegate Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetRewardTable* GetRewardTable();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	

};


UCLASS()
class RM_API UTestReward : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UTestReward() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FReportUserFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UTestReward* SendTestReward(EGameType gameType, EGameRankType rankType);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	EGameType GameType;
	EGameRankType RankType;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateWinnerFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UUpdateWinner : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UUpdateWinner() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FUpdateWinnerFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UUpdateWinner* UpdateWinner(FString WinnerNum);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString Winner;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetCardRankArrayFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage,const TArray<FCardRankInfo>&,Data);

UCLASS()
class RM_API UGetCardRankArray : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetCardRankArray() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FGetCardRankArrayFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UGetCardRankArray* GetCardRankArray();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInputEmailFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UInputEmail : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UInputEmail() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FInputEmailFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UInputEmail* InputEmail(FString Email);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString Email;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGoogleLinkFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API UGoogleLinkAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGoogleLinkAPI() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FGoogleLinkFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UGoogleLinkAPI* SetGoogleLink(FString Link);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString Link;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSetCardTutorialFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

UCLASS()
class RM_API USetCardTutorialAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	USetCardTutorialAPI() {};

	UPROPERTY(BlueprintAssignable, Category="RMProtocol")
	FSetCardTutorialFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static USetCardTutorialAPI* USetCardTutorial(const int32 Tutorial);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 Tutorial;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDailyQuestFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, FDailyQuestInfo,QuestInfo);

UCLASS()
class RM_API UDailyQuestAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UDailyQuestAPI() {};

	UPROPERTY(BlueprintAssignable, Category="RMProtocol")
	FDailyQuestFinishedPin Finished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UDailyQuestAPI* DailyQuest();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FScreenDdisplayFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, const TArray<FScreenDisplayInfo>&,ScreenDisplayInfos);

UCLASS()
class RM_API UScreenDisplayAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UScreenDisplayAPI() {};

	UPROPERTY(BlueprintAssignable, Category="RMProtocol")
	FScreenDdisplayFinishedPin Finished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UScreenDisplayAPI* ScreenDisplay();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNPCTutorialFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);
UCLASS()
class RM_API UNPCTutorialAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UNPCTutorialAPI() {}

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FNPCTutorialFinishedPin Finished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UNPCTutorialAPI* NPCTutorialAPI(int32 npcTutorial);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 npcTutorial;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNPCIdFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);
UCLASS()
class RM_API UNPCIdAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UNPCIdAPI() {}

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FNPCIdFinishedPin Finished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UNPCIdAPI* NPCIdAPI(int32 npcChoice);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 npcChoice;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FActionPointAddFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, FActionPoint, ActionPoint);
UCLASS()
class RM_API UActionPointAdd : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UActionPointAdd() {}

	UPROPERTY(BlueprintAssignable, Category = "ActionPoint")
	FActionPointAddFinishedPin Finished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "ActionPoint")
	static UActionPointAdd* PointAdd();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FActionPointUseFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, FActionPoint, ActionPoint);
UCLASS()
class RM_API UActionPointUse : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UActionPointUse() {}

	UPROPERTY(BlueprintAssignable, Category = "ActionPoint")
	FActionPointUseFinishedPin Finished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "ActionPoint")
	static UActionPointUse* PointUse();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTutorialFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);
UCLASS()
class RM_API UTutorialAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UTutorialAPI() {}

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FNPCTutorialFinishedPin Finished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UTutorialAPI* TutorialAPI(FString type, int32 val);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString type;
	int32 val;
	
};


#pragma region Market

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNormalFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);

/**
 *
 */
UCLASS()
class RM_API URegisterMarket : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	URegisterMarket() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FNormalFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static URegisterMarket* RegisterMarket(FName ItemKey, FString Title, int32 StartPrice, int32 BuyPrice, int32 Period, EAuctionCategory Category);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FName ItemKey;
	FString Title;
	int32 StartPrice;
	int32 BuyPrice;
	int32 Period;
	EAuctionCategory Category;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FUnRegisterMarketFinishedPin, int32, ResultCode, FString, ErrMessage, FAuctionItem, UpdatedItem, int32, PenaltyCoin);

UCLASS()
class RM_API UUnRegisterMarket : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UUnRegisterMarket() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FUnRegisterMarketFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static UUnRegisterMarket* UnRegisterMarket(FString AuctionNumber);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString AuctionNumber;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAuctionItemFinishedPin, int32, ResultCode, FAuctionItem, Item, FString, ErrMessage);

UCLASS()
class RM_API UBidMarketItem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UBidMarketItem() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FAuctionItemFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static UBidMarketItem* BidMarketItem(FString AuctionNumber, int32 BidPrice);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString AuctionNumber;
	int32 BidPrice;
};


UCLASS()
class RM_API UWatchMarketItem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UWatchMarketItem() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FAuctionItemFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static UWatchMarketItem* WatchMarketItem(FString AuctionNumber, bool IsWatch);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString AuctionNumber;
	bool IsWatch;
};


UCLASS()
class RM_API UBuyMarketItem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UBuyMarketItem() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FAuctionItemFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static UBuyMarketItem* BuyMarketItem(FString AuctionNumber, int32 BuyNowPrice);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	FString AuctionNumber;
	int32 BuyNowPrice;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetMarketItemsFinishedPin, int32, ResultCode, FString, ResultMessage, int32, TotalPage, const TArray<FAuctionItem>&, ItemList);

/**
 *
 */
UCLASS()
class RM_API UGetMarketItems : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetMarketItems() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetMarketItemsFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static UGetMarketItems* GetMarketItems(EAuctionCategory Category, EMarketSorting Sorting, int32 Page, int32 Count);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	EAuctionCategory Category;
	EMarketSorting Sorting;
	int32 Page;
	int32 Count;
};


UCLASS()
class RM_API UGetMyMarketItems : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetMyMarketItems() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetMarketItemsFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static UGetMyMarketItems* GetMyMarketItems(EMyMarketCategory Category, int32 Page, int32 Count);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	EMyMarketCategory Category;
	int32 Page;
	int32 Count;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetMyWatchFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, const TArray<FString>&, Items);

UCLASS()
class RM_API UGetMyWatchItem : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetMyWatchItem() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetMyWatchFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Market")
		static UGetMyWatchItem* GetMyWatchItem();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};
#pragma endregion

#pragma region Costumization

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAvatarCustomizingPin, int32, ResultCode, bool, Success, FString, ErrMessage);
UCLASS()
class RM_API UAvatarCustomizing : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UAvatarCustomizing(){};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FAvatarCustomizingPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|User")
	static UAvatarCustomizing* AvatarCustomizing(); 
	
	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

#pragma endregion

#pragma region Membership
UCLASS()
class RM_API UCheckAccountEnd : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UCheckAccountEnd() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FCreateAccountFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|User")
		static UCheckAccountEnd* CheckAccountEnd();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};
#pragma endregion

#pragma region FIRST_REWARD
UCLASS()
class RM_API UFirstRewardFirebase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFirstRewardFirebase() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FCreateAccountFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol|Reward")
		static UFirstRewardFirebase* CheckCanGetRewardFirebase();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};
#pragma endregion

/* DEPRECATED RM_EVENT REGION */
#pragma region RM_EVENT
/* class will be DEPRECATED when web browser system uploaded. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGetEventListFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage);
UCLASS()
class RM_API UGetEventList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UGetEventList() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetEventListFinishedPin Finished;
private:
	class URMEventSubSystem* RMEventSubSystem;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "RMProtocol|Event")
		static UGetEventList* GetEventList(UObject* WorldContextObject);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAttendEventFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, FMyEventAttendanceInfo, myAttendanceInfo);
UCLASS()
class RM_API UAttendEvent : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UAttendEvent() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FAttendEventFinishedPin Finished;
private:
	class URMEventSubSystem* RMEventSubSystem;
	int eventSeq;
	FString Val_1;
	FString Val_2;
private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "RMProtocol|Event")
		static UAttendEvent* AttendEvent(UObject* WorldContextObject, int eventSeq, FString val, FString val2);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEventWinnerFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage, FEventWinnerInfo, EventWinnerInfo);
UCLASS()
class RM_API UEventWinner : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UEventWinner() {}

	UPROPERTY(BlueprintAssignable, Category = "RMProtocl")
	FEventWinnerFinishedPin Finished;

private:
	class URMEventSubSystem* RMEventSubSystem;
	int eventSeq;
	FString Value;
	FString Value2;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "RMProtocol|Event")
	static UEventWinner* EventWinner(UObject* WorldContextObject, int EventSeq, FString Value, FString Value2);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

#pragma endregion

#pragma region RM_NOTICE
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FGetNoticeListFinishedPin, int32, ResultCode, bool, Success, FString, ErrMessage,
	int32, TotalPage, int32, CurrentPage, const TArray<FNoticeBoardInfo>&, BoardListInfo);
UCLASS()
class RM_API UGetNoticeBoardList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UGetNoticeBoardList() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FGetNoticeListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetNoticeBoardList* GetNoticeBoardList(int32 ShowPage, int32 CountOnce, FString SortCountryCode, ENoticeBoardType boardType = ENoticeBoardType::News);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

private:
	int32 page;
	int32 showCount;
	FString countryCode;
	ENoticeBoardType noticeBoardtype;
};


#pragma endregion 

#pragma region PostBase64ImageToServer
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FPostBase64DataToServerFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, FString, ImageUrl, FString, ThumbnailUrl);

UCLASS()
class RM_API UPostBase64ImageToServer : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostBase64ImageToServer() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FPostBase64DataToServerFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UPostBase64ImageToServer* PostBase64ImageToServer(const FString& Base64);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	FString Base64;
};

#pragma endregion

#pragma region PostRegistPhotoData
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPostRegistPhotoDataFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, int, sessionId);

UCLASS()
class RM_API UPostRegistPhotoData : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	public:
	UPostRegistPhotoData() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
	FPostRegistPhotoDataFinishedPin Finished;

	private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
	static UPostRegistPhotoData* PostRegistPhotoData(const FString& imageUrl, int frameNumber);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	FString ImageUrl;
	int FrameNumber;
};

#pragma endregion

#pragma region GetPhotoList
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FGetPhotoListFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, const TArray<FPhotoInfo>&, PhotoInfos, int32, TotalCount, int32, LastPage);

UCLASS()
class RM_API UGetPhotoList : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetPhotoList() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetPhotoListFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetPhotoList* GetPhotoList(int32 Page, int32 Limit);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	int32 Page;
	int32 Limit;
};

#pragma endregion

#pragma region PostPhotoLike
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FPostPhotoLikeFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, bool, IsSelected, int32, CountLike);

UCLASS()
class RM_API UPostPhotoLike : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostPhotoLike() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FPostPhotoLikeFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UPostPhotoLike* PostPhotoLike(int32 Seq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	int32 Seq;
};

#pragma endregion

#pragma region PostPhotoReport
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPostPhotoReportFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage);

UCLASS()
class RM_API UPostPhotoReport : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPostPhotoReport() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FPostPhotoReportFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UPostPhotoReport* PostPhotoReport(int32 Seq);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	int32 Seq;
};

#pragma endregion

#pragma region GetPhotoRankListOfWeek
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetPhotoRankListOfWeekFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, const TArray<FPhotoInfo>&, PhotoInfos);

UCLASS()
class RM_API UGetPhotoRankListOfWeek : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetPhotoRankListOfWeek() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetPhotoRankListOfWeekFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetPhotoRankListOfWeek* GetPhotoRankListOfWeek();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

#pragma endregion

#pragma region GetPhotoRankListOfWhole
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGetPhotoRankListOfWholeFinishedPin, bool, Success, int32, ResultCode, FString, ResultMessage, const TArray<FPhotoInfo>&, PhotoInfos);

UCLASS()
class RM_API UGetPhotoRankListOfWhole : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UGetPhotoRankListOfWhole() {};

	UPROPERTY(BlueprintAssignable, Category = "RMProtocol")
		FGetPhotoRankListOfWholeFinishedPin Finished;

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RMProtocol")
		static UGetPhotoRankListOfWhole* GetPhotoRankListOfWhole();

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};

#pragma endregion
