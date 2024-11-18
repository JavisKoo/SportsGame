// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseRMGameInstance.h"
#include "RMProtocolFunctionLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Http.h"

void UBaseRMGameInstance::OnMultipleAccessError_Implementation()
{

}

void UBaseRMGameInstance::BindMultipleAccessError()
{
	//URMProtocolFunctionLibrary::OnMultipleAccessErrorDelegate.AddRaw(this, &UBaseRMGameInstance::OnMultipleAccessError);
	URMProtocolFunctionLibrary::OnMultipleAccessErrorDelegate.AddUFunction(this, FName("OnMultipleAccessError"));
}

void UBaseRMGameInstance::OnCheckServer_Implementation(FDateTime WorkoutTime)
{

}

void UBaseRMGameInstance::BindCheckServer()
{
	//URMProtocolFunctionLibrary::OnMultipleAccessErrorDelegate.AddRaw(this, &UBaseRMGameInstance::OnMultipleAccessError);
	URMProtocolFunctionLibrary::OnCheckServerDelegate.AddUFunction(this, FName("OnCheckServer"));
}


void UBaseRMGameInstance::AddToViewportClientContents(UUserWidget* WidgetObject)
{
	if (WidgetObject == nullptr || !WidgetObject->IsInViewport())
	{
		UE_LOG(LogTemp, Log, TEXT("widgetObject == nullptr! || Not In Viewport"));
		return;
	}

	GetGameViewportClient()->AddViewportWidgetContent(WidgetObject->TakeWidget());
	//viewportClientWidgetArr.Add(widgetObject);
	UE_LOG(LogTemp, Warning, TEXT("widgetObject successfully Added to GameViewportClientWidget."));
}
void UBaseRMGameInstance::RemoveFromViewportClientContents(UUserWidget* WidgetObject)
{
	if (WidgetObject == nullptr)// || viewportClientWidgetArr.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("widgetObject == nullptr!"));
		return;
	}
	/*if (!viewportClientWidgetArr.Contains(widgetObject))
	{
		UE_LOG(LogTemp, Log, TEXT("NOT CONTAIN widgetObject in viewportClientWidgetArr"));
		return;
	}
	viewportClientWidgetArr.Remove(widgetObject);*/

	GetGameViewportClient()->RemoveViewportWidgetContent(WidgetObject->TakeWidget());
	//WidgetObject->RemoveFromViewport();
	UE_LOG(LogTemp, Warning, TEXT("widgetObject successfully removed at GameViewportClient"));
}

void UBaseRMGameInstance::CreateInvisibleWidget(int ZOrder)
{
	if (invisibleWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBaseRMGameInstance::InvisibleWidgetClass is Must Set in your instance class."));
		return;
	}
	
	TArray<UUserWidget*> invisibleWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), invisibleWidgets, invisibleWidgetClass);
	if (invisibleWidgets.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBaseRMGameInstance::InvisibleWidgetClass is already exist."));
		return;
	}
	
	UUserWidget* widget = CreateWidget(GetWorld(), invisibleWidgetClass);
	widget->AddToViewport(ZOrder);
}

void UBaseRMGameInstance::RemoveInvisibleWidget()
{
	TArray<UUserWidget*> invisibleWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), invisibleWidgets, invisibleWidgetClass);

	for (UUserWidget* it : invisibleWidgets)
		it->RemoveFromParent();
}

FDateTime UBaseRMGameInstance::GetServerUTCTime(const UObject* WorldContextObject)
{
	if (!bIsServerUTCSetting)
	{
		UE_LOG(LogTemp, Warning, TEXT("you must call InitServerUTCTime before call this Function. \nreturn UKismetSystemLibrary::UtcNow() instead."));
		return UKismetMathLibrary::UtcNow();
	}
	int32 seconds;
	float partialSeconds;
	UGameplayStatics::GetAccurateRealTime(seconds, partialSeconds);
	partialSeconds += seconds;

	float offsetPlayTime = partialSeconds - prevPlayTime;
	prevPlayTime = partialSeconds;
	ServerUTCTime += UKismetMathLibrary::FromSeconds(offsetPlayTime);

	return ServerUTCTime;
}

void UBaseRMGameInstance::InitServerUTCTime(const UObject* WorldContextObject, FDateTime serverTime)
{
	if (bIsServerUTCSetting)
	{
		UE_LOG(LogTemp, Warning, TEXT("offsetToServerTime Value is already Setting. \n ServerTime already Setted by application."));
		return;
	}
	int32 seconds;
	float partialSeconds;
	UGameplayStatics::GetAccurateRealTime(seconds, partialSeconds);
	partialSeconds += seconds;
	offsetToServerTime = prevPlayTime = partialSeconds;
	ServerUTCTime = serverTime;

	bIsServerUTCSetting = true;
}

void UBaseRMGameInstance::ResetServerUTCTime()
{
	bIsServerUTCSetting = false;
	ServerUTCTime = FDateTime();
	offsetToServerTime = 0.0f;
	prevPlayTime = 0.0f;
}


void UBaseRMGameInstance::TestLog(FString Log)
{
	TestLog2(FString::FromInt(URMProtocolFunctionLibrary::GetUserSeq()), Log);
}

void UBaseRMGameInstance::TestLog2(FString UserID, FString Log)
{
	auto HttpRequest = URMProtocolFunctionLibrary::GetDefaultHttp(TEXT("info/log"), TEXT("POST"));

	TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
	//_payloadJson->SetStringField("id", FString::FromInt(URMProtocolFunctionLibrary::GetUserSeq()));
	_payloadJson->SetStringField("id", *UserID);
	_payloadJson->SetStringField("log", *Log);

	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBaseRMGameInstance::Response_TestLog);
		UE_LOG(LogTemp, Log, TEXT("TestLog : %s"), *HttpRequest->GetURL());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TestLog - cannot process request"));
	}
}

void UBaseRMGameInstance::PlaySFXSound3D(USoundBase* sound, FVector location, FRotator rotation, 
	float volumeMultiplier /* = 1.0f */, float pitchMultiplier /* = 1.0f */, float startTime /* = 0.0f */,
	USoundAttenuation* attenuation /* = nullptr*/, USoundConcurrency* concurrency /* = nullptr*/, AActor* owner /* = nullptr*/)
{
	if (!IsValid(sound) || !bIsSFXPlay)
		return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), 
		sound, location, rotation,																/* Spawn Expr */
		volumeMultiplier, pitchMultiplier, startTime,											/* Volume Expr */
		attenuation, concurrency, owner															/* Sound Additional Expr */
	);
}

void UBaseRMGameInstance::Response_TestLog(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (Response == nullptr || !WasSuccessful || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("Response_TestLog Fail!!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Response_TestLog : %s"), *Response->GetContentAsString());
}