// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "RMProtocolFunctionLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRMProtocol, Log, All)
DECLARE_MULTICAST_DELEGATE(FOnMultipleAccessError);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCheckServer, FDateTime);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHttpRequestResponseDelegate, FString, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHttpRequestErrorDelegate, int, ErrorCode);


#define CREATE_SIMPLE_DELEGATE_LAMBDA(DelegatePtr, ...)																											\
	FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]																										\
	{																																							\
		if (DelegatePtr->IsBound())																																\
		{																																						\
			DelegatePtr->ExecuteIfBound(__VA_ARGS__);																											\
			DelegatePtr->Clear();																																\
		}																																						\
	})																																							\
																																								
#define EXECUTE_DYNAMIC_DELEGATE(DelegateClass, DelegateRef, ...)																								\
{																																								\
	DelegateClass* DelegatePtr = &DelegateRef;																													\
	if (DelegatePtr != nullptr && DelegatePtr->IsBound())																										\
	{																																							\
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(																									\
			CREATE_SIMPLE_DELEGATE_LAMBDA(DelegatePtr, __VA_ARGS__),																							\
			TStatId(), nullptr, ENamedThreads::GameThread																										\
		); 																																						\
	}																																							\
}                                                                                                                                   							
																																								
#define CHECK_HTTP_RESPONSE(HttpResponsePtr, DelegateClass, DynamicDelegate, ...)																				\
{																																								\
	if (HttpResponsePtr == nullptr)																																\
	{																																							\
		EXECUTE_DYNAMIC_DELEGATE(DelegateClass, DynamicDelegate, __VA_ARGS__);																					\
		return;																																					\
	}																																							\
																																								\
	if (HttpResponsePtr->GetResponseCode() != 200)																												\
	{																																							\
		FString Msg = FString::Printf(TEXT("Error processing request.\nProtocol: %s\nError Code: %d"),															\
			*Response->GetURL(), HttpResponsePtr->GetResponseCode());																							\
		UE_LOG(LogRMProtocol, Warning, TEXT("%s"), *Msg);																										\
		EXECUTE_DYNAMIC_DELEGATE(DelegateClass, DynamicDelegate, __VA_ARGS__);																					\
		return;																																					\
	}																																							\
}

/**
 * 
 */
UCLASS()
class RM_API URMProtocolFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SetBaseUrl(const FString& url);
	static const FString GetBaseUrl();
	static void SetAccessToken(const FString& token);
	static const FString GetAccessToken();
	static FString GetProtocolErrorMessage(const FHttpResponsePtr Response, UClass* classType);
	static bool CheckResponse(const FHttpResponsePtr Response, bool WasSuccessful, UClass* ClassType, int32& ErrorCode, FString& ErrorMsg);
	static void SetEncryptionUserSeq(FString encSeq);
	static bool CheckSubErrorCode(TSharedPtr<FJsonObject> json);
	static void SetDefaultHeader(FHttpRequestPtr HttpRequest);
	static FHttpRequestPtr GetDefaultHttp(const FString& URL, const FString& Method);
	static void ProcessDefaultHttp(FHttpRequestPtr HttpRequest, FOnHttpRequestResponseDelegate& OnResponse, FOnHttpRequestErrorDelegate& OnError, TFunction<void(TSharedPtr<FJsonObject>)> SucceededFunction);
	
	/* UFUNCTION IMPLEMETATION */
	UFUNCTION(BlueprintCallable, Category = "RMProtocol")
	static void SetUserSeq(int32 UserSeq);
	
	UFUNCTION(BlueprintCallable, Category = "RMProtocol")
	static int32 GetUserSeq();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="RMProtocol")
	static FString GetEncryptionUserSequence();
	
	UFUNCTION(BlueprintCallable, Category = "RMProtocol")
	static void SetBackdoor(bool IsBackdoor);
private:
	static FString _BaseUrl;
	static FString _Token;
	static int32 _UserSeq;

	/* Encryption UserSeq Data */
	static FString _EncryptionUserSeq;
	
	static bool _IsBackdoor;

public:
	static FOnMultipleAccessError OnMultipleAccessErrorDelegate;
	static FOnCheckServer OnCheckServerDelegate;
};