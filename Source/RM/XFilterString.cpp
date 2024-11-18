// Fill out your copyright notice in the Description page of Project Settings.
#include "XFilterString.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"



UXFilterString::UXFilterString() {
	//HttpModule = &FHttpModule::Get();
	LoadForbiddenWord();
}


//void UXFilterString::ExecuteForbiddenWordDownload(FString sSourceURL)
//{
//	this->SourceURL = sSourceURL;
//
//	TSharedPtr<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
//	HttpRequest->SetVerb("GET");
//	HttpRequest->SetURL(this->SourceURL);
//	//HttpRequest->OnRequestProgress().BindUObject(this, &UHttpDownloader::HttpRequestProgressDelegate);
//	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXFilterString::HttpRequestFinishedDelegate);
//
//	HttpRequest->ProcessRequest();
//}

void UXFilterString::LoadForbiddenWord()
{
	if (ForbiddenWordString.IsEmpty())
		ForbiddenWordString = FText::FromStringTable("/Game/RM/DataTables/ForbiddenWord.ForbiddenWord", "ForbiddenWord").ToString();

	FName TableID = "/Game/RM/DataTables/ForbiddenWord_V2.ForbiddenWord_V2";
	if (ForbiddenWord_KR.IsEmpty())
	{
		ForbiddenWord_KR = FText::FromStringTable(TableID, "ForbiddenWord_KR").ToString();
		ConvertRegexString(ForbiddenWord_KR);
	}
		
	if (ForbiddenWord_EN.IsEmpty())
	{
		ForbiddenWord_EN = FText::FromStringTable(TableID, "ForbiddenWord_EN").ToString();
		ConvertRegexString(ForbiddenWord_EN);
	}
		
	if (ForbiddenWord_CN.IsEmpty())
	{
		ForbiddenWord_CN = FText::FromStringTable(TableID, "ForbiddenWord_CN").ToString();
		ConvertRegexString(ForbiddenWord_CN);
	}
	
	if (ForbiddenWord_SP.IsEmpty())
	{
		ForbiddenWord_SP = FText::FromStringTable(TableID, "ForbiddenWord_SP").ToString();
		ConvertRegexString(ForbiddenWord_SP);
	}

	if (ForbiddenWord_Essential.IsEmpty())
	{
		ForbiddenWord_Essential = FText::FromStringTable(TableID, "ForbiddenWord_Essential").ToString();
		ForbiddenWord_Essential += TEXT("|") + ForbiddenWord_EN + TEXT("|") + ForbiddenWord_SP;
		ConvertRegexString(ForbiddenWord_Essential);
	}
		
	if (ForbiddenWord_All.IsEmpty())
		ForbiddenWord_All = ForbiddenWord_KR + TEXT("|") + ForbiddenWord_EN + TEXT("|") + ForbiddenWord_CN + TEXT("|") + ForbiddenWord_SP + TEXT("|") + ForbiddenWord_Essential;
}

void UXFilterString::ConvertRegexString(FString& inString)
{
	FString cpyForbidden = TEXT("(") + inString + TEXT(")");
	cpyForbidden = cpyForbidden.Replace(TEXT(","), TEXT(")|("));
	inString = cpyForbidden;
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *cpyForbidden);
}

//void UXFilterString::HttpRequestFinishedDelegate(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
//{
//	bool bResult = false;
//	FString ResponseStr, ErrorStr;
//
//	if (bSucceeded && HttpResponse.IsValid())
//	{
//		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
//		{
//			ForbiddenWordString = HttpResponse->GetContentAsString();
//
//			bResult = true;
//		}
//		else
//		{
//			ErrorStr = FString::Printf(TEXT("Invalid response. code=%d error=%s"),
//				HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());
//		}
//	}
//	else
//	{
//		ErrorStr = TEXT("No response");
//	}
//	if (!ErrorStr.IsEmpty())
//	{
//		UE_LOG(LogTemp, Log, TEXT("ReadFile request failed. %s"), *ErrorStr);
//	}
//
//	XFilterOnReadFileCompleteDelegates.Broadcast(bResult, ForbiddenWordString);
//}

EXFilterErrorCode UXFilterString::CheckNickName(const FString& InputString)
{
	if (InputString.Len() < 2)
	{
		return EXFilterErrorCode::E_4;
	}
	else if (InputString.Len() > 10)
	{
		return EXFilterErrorCode::E_5;
	}

#pragma region REGEX_SUMMARY
	/*
		<UE4 Regex c++>
		-기본적으로 C++ std::regex와 문법을 같이 한다.-
		-FRegexPattern Pattern(TEXT("")) 선언 후, FRegexMatcher를 이용하여 정규식 검사를 진행할 수 있다.-

		1) [] 안에 원하는 문자를 넣어 행에서 모두 찾을 수 있도록 만듬.
		2) a-z와 같이 영역을 지정할 수 있다.

		3) '|'(OR):	연산자를 사용하여 대괄호 안에서 원하는 모든 영역을 지정할 수 있다.
			ex) TEXT("[a-z|0-9]"): a~z or 0~9를 찾음.

		4) '^'(XOR): 연산자를 사용하여 시작 문자를 알릴 수 있다.
			ex) TEXT("^[a-z]"): a-z로 시작하는 문자열을 찾음.

		5) '$':	연산자를 사용하여 끝 문자를 알릴 수 있다.
			ex) TEXT("[a-z]&"): a-z로 끝나는 문자열을 찾음.
			ex) TEXT("^[a-z]|[a-z]$") == a-z로 시작하거나 끝나는 문자를 찾음. (중간에 끼는 건 상관 X)

		6) '{N}': 해당 규칙을 이용하여 해당 문자가 N회 반복되었는지 확인한다.
			ex) TEXT("b{3}"): 'b'가 3회 반복("bbb")되었는지 확인한다. ('ababab'는 잡아내지 못함)
			ex) TEXT("[a-z]{3}"): a-z사이의 문자가 3번 반복되었는지 확인한다.

		7) '{N,}': 해당 규칙을 이용하여 해당 문자가 'n'회 이상 반복되었는지 확인한다.
			- {N}과의 차이점이 뭔지 모르겠다.(?)

		8) '{N,M}': 해당 규칙을 이용하여 해당 문자게 'n'회 이상, 'm'회 아하 반복되었는지 확인한다.
			- 언리얼에서는 동작을 안하는 것 같다.(?)

		9) '[^]': NOT을 의미한다.
			ex) TEXT("[^[a-z]]"): a-z를 '제외'한 모든 문자와 매칭한다. (a-z문자로만 작성되었는지 확인한다.)

		10) '()': 그룹을 표현하여 문자열을 나타냄. (괄호 안에 들어간 문자열을 하나로 취급)
			ex) TEXT("(abc){3}"): 'abcabcabc'와 같은 패턴을 검출하기 위함.

		<UTF>
		u3130-u318f : u3131(ㄱ) ~ u318c(ㆌ)
		u2000대 : 특수문자

		// 이후 업데이트 //
	*/
#pragma endregion
	
	// 공백
	FRegexPattern MatherPatter = FRegexPattern(FString(TEXT("\\s")));
	FRegexMatcher Matcher(MatherPatter, InputString);
	if (Matcher.FindNext())
	{
		return EXFilterErrorCode::E_3;
	}
	
	// 특수문자
	//const FRegexPattern myPattern(TEXT("[\"{}/?.,;:|()*~`!^\\-_+┼<>@#$%&'""\\=\\]\\[]"));
	//const FRegexPattern myPattern(TEXT("[\"\']"));
	// const FRegexPattern myPattern(TEXT("(abc){3}"));
	// [ㄱ-ㅎ~!@\#$%^&*\()\=+|\\/:;?""<>'\u3130-\u318f\s*]

	// 한글 모음 + 자음 & 키보드 기본 특수문자
	const FRegexPattern myPattern(TEXT("[\u3130-\u318f|\\uD83C-\\uDBFF\\uDC00-\\uDFFF|\"{}/?.,;:|()*~`@!^\\-_+┼<>@#$%&'""\\=\\]\\[]+"));
	// const FRegexPattern EmojiPattern(TEXT("[\\uD83C-\\uDBFF\\uDC00-\\uDFFF]+"));
	FRegexMatcher myMatcher(myPattern, InputString);
	if (myMatcher.FindNext())
	{
		int32 b = myMatcher.GetMatchBeginning();
		int32 e = myMatcher.GetMatchEnding();
		//UE_LOG(LogTemp, Warning, TEXT("REGEX %i %i"), b, e);
		return EXFilterErrorCode::E_2;
	}
	
	/* 금칙어 추가 (JUNHO) */
	const FRegexPattern myPattern_Forbidden(ForbiddenWord_All.ToUpper());
	FString cpyStr = InputString.ToUpper();
	FRegexMatcher matcher_Forbidden(myPattern_Forbidden, cpyStr);
	if (matcher_Forbidden.FindNext())
	{
		int32 b = myMatcher.GetMatchBeginning();
		int32 e = myMatcher.GetMatchEnding();
		//UE_LOG(LogTemp, Warning, TEXT("REGEX %i %i"), b, e);
		return EXFilterErrorCode::E_6;
	}

	////////////////////////////////////////////////////////////////// ::::::::ORIGIN CODE::::::::: //////////////////////////////////////////////////////////////////////
	//if (ForbiddenWordMatch(ForbiddenWordString, InputString))
	//{
	//	return EXFilterErrorCode::E_6;
	//}
	
	return EXFilterErrorCode::E_0;
}



bool UXFilterString::RegexMatch(const FString& InputString, const FString& Pattern, TArray<FString>& Result)
{
	FRegexPattern MatherPatter(Pattern);
	FRegexMatcher Matcher(MatherPatter, InputString);

	while (Matcher.FindNext())
	{
		Result.Add(Matcher.GetCaptureGroup(0));
	}

	return Result.Num() == 0 ? false : true;
}


bool UXFilterString::ForbiddenWordMatch(FString SearchString, FString InputString)
{
	if (InputString.Len() <= 0 || InputString.Len() <= 0) return false;

	// char* txt = TCHAR_TO_UTF8(*SearchString);
	// char* pat = TCHAR_TO_UTF8(*InputString);

	//UE_LOG(LogTemp, Log, TEXT("%s"), *SearchString);
	
	int32 n = KMPSearch(InputString, SearchString);
	//UE_LOG(LogTemp, Log, TEXT("금지어 검색된 인덱스 : %d"), n);
	return n == 0 ? false : true;
}

FString UXFilterString::GetForbiddenString(EXfilterForbbidenType forbiddenType)
{
	switch (forbiddenType)
	{
		case EXfilterForbbidenType::TYPE_All:
			return ForbiddenWord_All;
		case EXfilterForbbidenType::TYPE_KR:
			return ForbiddenWord_KR;
		case EXfilterForbbidenType::TYPE_EN:
			return ForbiddenWord_EN;
		case EXfilterForbbidenType::TYPE_CN:
			return ForbiddenWord_CN;
		case EXfilterForbbidenType::TYPE_SP:
			return ForbiddenWord_SP;
		case EXfilterForbbidenType::TYPE_ESSENTIAL:
			return ForbiddenWord_Essential;
	}

	return ForbiddenWord_All;
}

// C++ program for implementation of KMP pattern searching
// algorithm

// Prints occurrences of txt[] in pat[]
int32 UXFilterString::KMPSearch(FString pat, FString txt)
{
	// int32 M = strlen(pat);
	// int32 N = strlen(txt);
	int32 M = pat.Len();
	int32 N = txt.Len();

	// create lps[] that will hold the longest prefix suffix
	// values for pattern
	//int32 lps[M];
	int32* lps = new int32[M];

	// Preprocess the pattern (calculate lps[] array)
	computeLPSArray(pat, M, lps);

	int32 result = 0;
	int32 i = 0; // index for txt[]
	int32 j = 0; // index for pat[]
	while (i < N) {
		if (pat[j] == txt[i]) {
			j++;
			i++;
		}

		if (j == M) {
			result = i - j;
			//UE_LOG(LogTemp, Log, TEXT("Found pattern at index %d "), result);
			j = lps[j - 1];
		}

		// mismatch after j matches
		else if (i < N && pat[j] != txt[i]) {
			// Do not match lps[0..lps[j-1]] characters,
			// they will match anyway
			if (j != 0)
				j = lps[j - 1];
			else
				i = i + 1;
		}
	}

	delete[] lps;

	return result;
}

// Fills lps[] for given patttern pat[0..M-1]
void UXFilterString::computeLPSArray(FString pat, int32 M, int32* lps)
{
	// length of the previous longest prefix suffix
	int32 len = 0;

	lps[0] = 0; // lps[0] is always 0

	// the loop calculates lps[i] for i = 1 to M-1
	int32 i = 1;
	while (i < M) {
		if (pat[i] == pat[len]) {
			len++;
			lps[i] = len;
			i++;
		}
		else // (pat[i] != pat[len])
		{
			// This is tricky. Consider the example.
			// AAACAAAA and i = 7. The idea is similar
			// to search step.
			if (len != 0) {
				len = lps[len - 1];

				// Also, note that we do not increment
				// i here
			}
			else // if (len == 0)
			{
				lps[i] = 0;
				i++;
			}
		}
	}
}

bool UXFilterString::ValidInputString(EMatchPatternType MatchPattern, const FString& InputString)
{
	FString Pattern;
	switch (MatchPattern)
	{
		case EMatchPatternType::Email: Pattern = TEXT("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"); break;
	}
	
	const FRegexPattern RegexPattern(Pattern);
	FRegexMatcher RegexMatcher(RegexPattern, InputString);
	if (RegexMatcher.FindNext())
	{
		int32 b = RegexMatcher.GetMatchBeginning();
		int32 e = RegexMatcher.GetMatchEnding();
		//UE_LOG(LogTemp, Warning, TEXT("REGEX %i %i"), b, e);
		return true;
	}

	return false;
}