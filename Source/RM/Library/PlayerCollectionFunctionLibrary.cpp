#include "PlayerCollectionFunctionLibrary.h"
#include "RM/Data/NetworkData.h"
#include "SaveFunctionLibrary.h"
#include "Engine/DataTable.h"

const int32 UPlayerCollectionFunctionLibrary::MaxAcquirablePlayerPieceCount = 10;

TArray<FName> UPlayerCollectionFunctionLibrary::PlayerDataRowNames;
int32 UPlayerCollectionFunctionLibrary::AcquirablePlayerRowNameIndex = 0;

const FPlayerUserData UPlayerCollectionFunctionLibrary::GetPlayerUserData(const int32 PlayerId)
{
	TArray<FStringFormatArg> FormatArg = { FPaths::ProjectDir(), FString::FromInt(PlayerId) };
	FString JsonPath = FString::Format(TEXT("{0}/Database/{1}.json"), FormatArg);
	return USaveFunctionLibrary::GetPlayerUserDataByJson(JsonPath);
}

int32 UPlayerCollectionFunctionLibrary::GetAcquirablePlayerId()
{
	if (PlayerDataRowNames.Num() == 0)
	{
		UDataTable* PlayerDataTable = LoadObject<UDataTable>(NULL, TEXT("/Game/Dev/CollectionUI/Resource/CCollectionPlayerInfoList.CCollectionPlayerInfoList"), NULL, LOAD_None, NULL);
		if (PlayerDataTable)
		{
			PlayerDataRowNames = PlayerDataTable->GetRowNames();
		}
	}

	int32 PlayerDataRowNameCount = PlayerDataRowNames.Num();
	for (int32 PlayerDataRowNameIndex = AcquirablePlayerRowNameIndex; PlayerDataRowNameIndex < PlayerDataRowNameCount; ++PlayerDataRowNameIndex)
	{
		const FName& PlayerDataRowName = PlayerDataRowNames[PlayerDataRowNameIndex];
		const int32 PlayerId = FCString::Atoi(*PlayerDataRowName.ToString());
		const FPlayerUserData& PlayerUserData = GetPlayerUserData(PlayerId);

		if (PlayerUserData.PlayerPieceCount < MaxAcquirablePlayerPieceCount && !PlayerUserData.bAcquisition)
		{
			AcquirablePlayerRowNameIndex = PlayerDataRowNameIndex + 1;
			AcquirablePlayerRowNameIndex %= PlayerDataRowNameCount;

			return PlayerId;
		}
	}

	return INDEX_NONE;
}
