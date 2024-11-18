#include "CSavePlayer.h"
#include "UnrealClient.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"

ACSavePlayer::ACSavePlayer()
{
}

void ACSavePlayer::SavePlayerCards(FName UserNickname, FName CardRowName)
{
	// Get으로 Json 얻어와서 Tarray 읽어서 거기서 저장

	FString JsonString;

	FCardIndexData Data;

	TArray<FName> PlayerCardNameArray = LoadPlayerCardsArray(UserNickname);
	for (FName name : PlayerCardNameArray)
	{
		Data.PlayerList.Add(name);
	}

	Data.PlayerList.Add(CardRowName);

	FJsonObjectConverter::UStructToJsonObjectString(Data, JsonString);
	FString FilePath = FPaths::ProjectDir() + FString(TEXT("/Saved/Json/CardData/"))
		+ UserNickname.ToString();
	UE_LOG(LogTemp, Log, TEXT("ACSavePlayer::SavePlayerCards - FilePath : %s"), *FilePath);
	FString JsonPath = FPaths::ConvertRelativePathToFull(FilePath);
	if (FFileHelper::SaveStringToFile(*JsonString, *JsonPath))
	{
		UE_LOG(LogTemp, Log, TEXT("ACSavePlayer::SavePlayerCards success"));
	}	
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ACSavePlayer::SavePlayerCards false"));
	}
}

TArray<FName> ACSavePlayer::LoadPlayerCardsArray(FName UserNickname)
{
	TArray<FName> TestArray;

	FString JsonString;
	FString FilePath = FPaths::ProjectDir() + FString(TEXT("/Saved/Json/CardData/"))
		+ UserNickname.ToString();
	UE_LOG(LogTemp, Log, TEXT("ACSavePlayer::LoadPlayerCardsArray - FilePath : %s"), *FilePath);
	FString JsonPath = FPaths::ConvertRelativePathToFull(FilePath);

	if (FPaths::FileExists(*JsonPath))
	{
		UE_LOG(LogTemp, Log, TEXT("ACSavePlayer::LoadPlayerCardsArray Exist!!"));

		FFileHelper::LoadFileToString(JsonString, *JsonPath);

		UE_LOG(LogTemp, Log, TEXT("ACSavePlayer::LoadPlayerCardsArray - JsonString = : %s"), *JsonString);

		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			TArray<TSharedPtr<FJsonValue>> CardData = JsonObject->GetArrayField(TEXT("PlayerList"));
			for (auto& Element : CardData)
			{
				TestArray.Add(FName(Element->AsString()));

				UE_LOG(LogTemp, Log, TEXT("ACSavePlayer::LoadPlayerCardsArray [] : %s"), *Element->AsString());
			}
		}
	}

	return TestArray;
}


//If the BeginPlay is removed, the DBTween plugin will be not loaded.
void ACSavePlayer::BeginPlay()
{
	Super::BeginPlay();
}