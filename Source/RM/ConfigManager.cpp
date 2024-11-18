#include "ConfigManager.h"
#include "GeneralProjectSettings.h"


UConfigManager::UConfigManager() {
	HttpModule = &FHttpModule::Get();
	ContentString.Empty();
	State = 0;
}

void UConfigManager::ExecuteDownload(FString sSourceURL)
{
	ContentString.Empty();
	State = 1;

	auto HttpRequest = HttpModule->CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(sSourceURL);
	//HttpRequest->OnRequestProgress().BindUObject(this, &UHttpDownloader::HttpRequestProgressDelegate);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UConfigManager::HttpRequestFinishedDelegate);
	HttpRequest->ProcessRequest();
}

void UConfigManager::HttpRequestFinishedDelegate(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	bool bResult = false;
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			UE_LOG(LogTemp, Log, TEXT("ReadFile request complete. url=%s code=%d"),
				*HttpRequest->GetURL(), HttpResponse->GetResponseCode());

			ContentString = HttpResponse->GetContentAsString();

			bResult = true;

			State = 2;
		}
		else
		{
			ErrorStr = FString::Printf(TEXT("Invalid response. code=%d error=%s"),
				HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());
			State = 9;
		}
	}
	else
	{
		ErrorStr = TEXT("No response");
		State = 9;
	}

	if (!ErrorStr.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("ReadFile request failed. %s"), *ErrorStr);
		State = 9;
	}

	OnDownloadCompleteDelegates.Broadcast(bResult, ContentString);
}

FString UConfigManager::FileLoad(FString Path)
{
	FString Result;

	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*Path))
	{
		FFileHelper::LoadFileToString(Result, *Path);
	}

	return Result;
}

FConfigStruct UConfigManager::FileParsing(FString PlatformString, FString FullPath, EConfigType DefaultType)
{
	UE_LOG(LogTemp, Log, TEXT("HttpParsing PlatformString = %s"), *PlatformString);

	if (PlatformString == TEXT("Android")) ConfigType = EConfigType::E_Andorid;
	else if (PlatformString == TEXT("iOS")) ConfigType = EConfigType::E_iOS;
	else ConfigType = DefaultType;

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(FileLoad(FullPath));
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		GenerateStructsFromJson(ConfigStruct, JsonObject);
	}
	else {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Not Deserialize")); }
	}

	return ConfigStruct;
}


FConfigStruct UConfigManager::HttpParsing(FString PlatformString, EConfigType DefaultType)
{
	UE_LOG(LogTemp, Log, TEXT("HttpParsing PlatformString = %s"), *PlatformString);

	if (PlatformString == TEXT("Android")) ConfigType = EConfigType::E_Andorid;
	else if (PlatformString == TEXT("iOS")) ConfigType = EConfigType::E_iOS;
	else ConfigType = DefaultType;

	UE_LOG(LogTemp, Log, TEXT("HttpParsing = %s"), *ContentString);

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ContentString);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		//if (GEngine) {	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Deserialize"));	}

		GenerateStructsFromJson(ConfigStruct, JsonObject);
	}
	else {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Not Deserialize")); }
	}

	return ConfigStruct;
}


void UConfigManager::ManifestArrayInfo(TArray<FString>& ArrayManifestURL, FString& CloudURL, FString& IntallDirectory)
{
	IntallDirectory = ConfigStruct.InstallDir;
	CloudURL = ConfigStruct.CloudURL;

	for (int32 i = 0; i < ConfigStruct.ManifestArray.Num(); i++)
	{
		FString dir = ConfigStruct.ManifestDir + ConfigStruct.ManifestArray[i];
		ArrayManifestURL.Push(dir);
	}
}

bool UConfigManager::VersionCheck(FConfigStruct config)
{
	bool BResult = true;
	// 프로젝트 세팅의 버젼정보 얻기
	FString version = GetVersionString();

	//FString version2 = GetDefault<FPlatformProperties>()->
	UE_LOG(LogTemp, Log, TEXT("CheckComplete Config Version = %d.%d.%d / Project Version = %s"),
		config.Major, config.Minor, config.Revision, *version);

	TArray<FString> Array = {};
	version.ParseIntoArray(Array, TEXT("."));

	if (Array.Num() >= 2)
	{
		int32 Major = FCString::Atoi(*Array[0]);
		int32 Minor = FCString::Atoi(*Array[1]);
		if (config.Major != Major || config.Minor != Minor)
		{
			BResult = false;
		}
	}

	return BResult;
}

int32 UConfigManager::GetState()
{
	return State;
}

bool UConfigManager::IsRequestCompleted()
{
	return State == 2 || State == 9 ? true : false;
}

FString UConfigManager::GetVersionString()
{
	FString version;

#if PLATFORM_WINDOWS
	if (ConfigType == EConfigType::E_iOS)	version = GetMutableDefault<UIOSRuntimeSettings>()->VersionInfo;
	else version = GetMutableDefault<UAndroidRuntimeSettings>()->VersionDisplayName;
#elif PLATFORM_ANDROID
	version = GetMutableDefault<UAndroidRuntimeSettings>()->VersionDisplayName;
#elif PLATFORM_IOS
	version = GetMutableDefault<UIOSRuntimeSettings>()->VersionInfo;
#endif

	return version;
}


/*
** private
*/


void UConfigManager::GenerateStructsFromJson(
	FConfigStruct& ConfigStructs,
	TSharedPtr<FJsonObject> JsonObject
)
{
	int32 i = 0;
	if (ConfigType == EConfigType::E_iOS) i = 1;

	UE_LOG(LogTemp, Log, TEXT("Json Index = %d"), i);

	TArray<TSharedPtr<FJsonValue>> objArray = JsonObject->GetArrayField(TEXT("Config"));
	if (i < objArray.Num())
	{
		TSharedPtr<FJsonValue> value = objArray[i];
		TSharedPtr<FJsonObject> json = value->AsObject();

		FString	Type = json->GetStringField(TEXT("Type"));

		UE_LOG(LogTemp, Log, TEXT("Type = %s"), *Type);

		TSharedPtr<FJsonObject> verJson = json->GetObjectField(TEXT("Version"));
		int32	Major = FCString::Atoi(*verJson->GetStringField(TEXT("Major")));
		int32	Minor = FCString::Atoi(*verJson->GetStringField(TEXT("Minor")));
		int32	Revision = FCString::Atoi(*verJson->GetStringField(TEXT("Revision")));

		//UE_LOG(LogTemp, Log, TEXT(" Version = %d.%d.%d"), Major, Minor, Revision);

		int32	ServerState = json->GetNumberField(TEXT("ServerState"));
		FString ServerIP = json->GetStringField(TEXT("ServerIP"));
		int32	ServerPort = json->GetNumberField(TEXT("ServerPort"));
		FString InstallDir = json->GetStringField(TEXT("InstallDir"));
		FString ManifestDir = json->GetStringField(TEXT("ManifestDir"));
		FString CloudURL = json->GetStringField(TEXT("CloudURL"));

		TArray<FString> manifestArray;
		TArray<TSharedPtr<FJsonValue>> objManifestArray = json->GetArrayField(TEXT("ManifestArray"));
		for (int32 i2 = 0; i2 < objManifestArray.Num(); i2++)
		{
			TSharedPtr<FJsonValue> value2 = objManifestArray[i2];
			manifestArray.Push(value2.Get()->AsString());
		}

		ConfigStructs = FConfigStruct::BuildConfigStruct(
			Major, Minor, Revision, ServerState, ServerIP, ServerPort, InstallDir, ManifestDir, CloudURL, manifestArray
		);
	}
}

