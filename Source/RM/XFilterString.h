// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//#include "Runtime/Online/HTTP/Public/Http.h"
#include "XFilterString.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHttpXFilterDownloadFinishedDelegate, bool, bIsSuccess, FString, BannedWordString);

UENUM(BlueprintType)
enum class EXFilterErrorCode : uint8
{
	E_0 UMETA(DisplayName = "Complete"),
	E_1 UMETA(DisplayName = "The nickname is already in use."),		/* 이미 사용중인 닉네임 입니다. */
	E_2 UMETA(DisplayName = "Special characters cannot be used."),	/* 특수문자는 사용할 수 없습니다. */
	E_3 UMETA(DisplayName = "Space characters are not allowed."),	/* 공백문자는 사용할 수 없습니다. */
	E_4 UMETA(DisplayName = "You must enter at least 2 characters."),	/* 최소 2자 이상 입력해야 됩니다. */
	E_5 UMETA(DisplayName = "You must enter up to 10 characters or less."), /* 최대 10자 이하로 입력해야 됩니다. */
	E_6 UMETA(DisplayName = "Forbidden words cannot be used."), /* 금지 된 단어를 사용할 수 없습니다. */
};

UENUM(BlueprintType)
enum class EXfilterForbbidenType : uint8
{
	TYPE_All,
	TYPE_KR,
	TYPE_EN,
	TYPE_SP,
	TYPE_CN,
	TYPE_ESSENTIAL,
};

UENUM(BlueprintType)
enum class EMatchPatternType : uint8
{
	Email,
};

UCLASS(Blueprintable, BlueprintType)
class RM_API UXFilterString : public UObject
{
	GENERATED_BODY()
		
public:
	UXFilterString();

	/*UFUNCTION(BlueprintCallable, DisplayName = "ExecuteForbiddenWordDownload", Category = "XFilterString")
		void ExecuteForbiddenWordDownload(FString sSourceURL);*/
		
	UFUNCTION(BlueprintCallable, DisplayName = "LoadForbiddenWord", Category = "XFilterString")
		void LoadForbiddenWord();

	/*UPROPERTY(BlueprintAssignable)
		FHttpXFilterDownloadFinishedDelegate XFilterOnReadFileCompleteDelegates;*/

	UFUNCTION(BlueprintCallable, DisplayName = "CheckNickName", Category = "XFilterString")
		EXFilterErrorCode CheckNickName(const FString& InputString);

	UFUNCTION(BlueprintCallable, DisplayName = "RegexMatch", Category = "XFilterString")
		bool RegexMatch(const FString& InputString, const FString& Pattern, TArray<FString>& Result);

	UFUNCTION(BlueprintCallable, DisplayName = "ForbiddenWordMatch", Category = "XFilterString")
		bool ForbiddenWordMatch(FString SearchString, FString InputString);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XFilterString")
		FString GetForbiddenString(EXfilterForbbidenType forbiddenType);

	UFUNCTION(BlueprintCallable, DisplayName= "ValidInputString", Category = "XFilterString")
		bool ValidInputString(EMatchPatternType MatchPattern, const FString& InputString);

private:
	void ConvertRegexString(FString& inString);
	//	void HttpRequestFinishedDelegate(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	//FHttpModule* HttpModule;

	/*UPROPERTY()
		FString SourceURL;*/

	FString ForbiddenWordString;

	int32 KMPSearch(FString pat, FString txt);
	void computeLPSArray(FString pat, int32 M, int32* lps);
	
	FString ForbiddenWord_All;
	FString ForbiddenWord_KR;
	FString ForbiddenWord_EN;
	FString ForbiddenWord_CN;
	FString ForbiddenWord_SP;
	FString ForbiddenWord_Essential;
};
