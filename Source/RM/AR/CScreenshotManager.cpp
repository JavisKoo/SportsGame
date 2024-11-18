#include "CScreenshotManager.h"
#include "UnrealClient.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"

const int32 ACScreenshotManager::MaxScreenshotCount = 36;

ACScreenshotManager::ACScreenshotManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACScreenshotManager::BeginPlay()
{
	Super::BeginPlay();
}

FString ACScreenshotManager::Screenshot()
{
	TArray<FString> ScreenshotNames;
	GetScreenshotNames(ScreenshotNames);

	//int32 ScreenshotCount = ScreenshotNames.Num();
	//if (ScreenshotCount >= MaxScreenshotCount)
	//{
	//	TArray<FStringFormatArg> FormatArg = { FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), ScreenshotNames[0] };
	//	//FString FirstScreenshotFileName = FString::Format(TEXT("{0}/Saved/Screenshots/{1}"), FormatArg);
	//	FString FirstScreenshotFileName = FString::Format(TEXT("{0}Saved/Screenshots/{1}"), FormatArg);
	//	IFileManager& FileManager = IFileManager::Get();
	//	FileManager.Delete(*FirstScreenshotFileName);
	//	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FirstScreenshotFileName);
	//}

	int LastIndex = GetScreenshotLastIndex() + 1;
	TArray<FStringFormatArg> FormatArg = { FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), FString::FromInt(LastIndex) };
	FString FileName = FString::Format(TEXT("{0}/Saved/Screenshots/ARScreenshot_{1}.png"), FormatArg);
	SaveScreenshotData(LastIndex);

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FileName);
	FScreenshotRequest screenshot_request = FScreenshotRequest();
	screenshot_request.RequestScreenshot(FileName, true, false);
	return FileName;
}

void ACScreenshotManager::GetScreenshotNames(TArray<FString>& OutSceenshotNames)
{
	IFileManager& FileManager = IFileManager::Get();
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + FString(TEXT("/Saved/Screenshots/ARScreenshot_*.png"));
	FileManager.FindFiles(OutSceenshotNames, *Path, true, false);
	OutSceenshotNames.Sort();
}

void ACScreenshotManager::GetScreenshotNamesAndroid(const FString& Path, const FString& Name, TArray<FString>& OutSceenshotNames)
{
	FString FullPath = Path + TEXT("/") + Name + TEXT("*.png");

	IFileManager& FileManager = IFileManager::Get();
	//FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + FString(TEXT("/Saved/Screenshots/ARScreenshot_*.png"));
	FileManager.FindFiles(OutSceenshotNames, *FullPath, true, false);
	OutSceenshotNames.Sort();
}

UTexture2D* ACScreenshotManager::GetTextureFromPNG(const FString& FileName)
{
	TArray<FStringFormatArg> FormatArg = { FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), FileName };
	FString FilePath = FString::Format(TEXT("{0}/Saved/Screenshots/{1}"), FormatArg);
	UTexture2D* MyTexture = FImageUtils::ImportFileAsTexture2D(FilePath);
	return MyTexture;
}

UTexture2D* ACScreenshotManager::GetTextureFromPNGAndroid(const FString& Path, const FString& FileName)
{
	FString FilePath = Path + TEXT("/") + FileName;

	//TArray<FStringFormatArg> FormatArg = { FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), FileName };
	//FString FilePath = FString::Format(TEXT("{0}/Saved/Screenshots/{1}"), FormatArg);
	UTexture2D* MyTexture = FImageUtils::ImportFileAsTexture2D(FilePath);
	return MyTexture;
}

void ACScreenshotManager::SaveScreenshotData(const int32 LastIndex)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FScreenshotData(LastIndex), JsonString);
	FString JsonPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + FString(TEXT("/Saved/Json/ScreenshotData.json"));
	FFileHelper::SaveStringToFile(*JsonString, *JsonPath);
}

int32 ACScreenshotManager::GetScreenshotLastIndex()
{
	int32 LastIndex = INDEX_NONE;
	FScreenshotData ScreenshotData;
	FString JsonString;
	FString JsonPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + FString(TEXT("/Saved/Json/ScreenshotData.json"));

	if (FPaths::FileExists(*JsonPath))
	{
		FFileHelper::LoadFileToString(JsonString, *JsonPath);

		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			LastIndex = JsonObject->GetIntegerField(TEXT("LastIndex"));
		}
	}
	else
	{
		FJsonObjectConverter::UStructToJsonObjectString(FScreenshotData(), JsonString);
		FFileHelper::SaveStringToFile(*JsonString, *JsonPath);
	}

	return LastIndex;
}
