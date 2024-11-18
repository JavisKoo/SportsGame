#include "PlayerBookManager.h"
#include "RM/Library/SaveFunctionLibrary.h"
#include "RM/Library/PlayerCollectionFunctionLibrary.h"
#include "RM/Data/PlayerUserData.h"

void UPlayerBookManager::RequestPlayerPieceAcquisitiion(const int32 PlayerId)
{
	if (OnPlayerPieceAcqusitionResponse.IsBound())
	{
		FPlayerUserResponseData PlayerUserResponseData = GetPlayerUserResponseData(PlayerId);
		PlayerUserResponseData.Status = ENetworkStatus::Success;
		OnPlayerPieceAcqusitionResponse.Broadcast(PlayerUserResponseData);
	}
}

void UPlayerBookManager::RequestPlayerAcquisition(const int32 PlayerId)
{
	if (OnPlayerAcquisitionResponse.IsBound())
	{
		FPlayerUserResponseData PlayerUserResponseData = GetPlayerUserResponseData(PlayerId);
		PlayerUserResponseData.Status = ENetworkStatus::Success;
		OnPlayerAcquisitionResponse.Broadcast(PlayerUserResponseData);
	}
}

const FPlayerUserResponseData UPlayerBookManager::GetPlayerUserResponseData(const int32 PlayerId)
{
	const FPlayerUserData PlayerUserData = UPlayerCollectionFunctionLibrary::GetPlayerUserData(PlayerId);
	return FPlayerUserResponseData(PlayerUserData, PlayerId);
}
