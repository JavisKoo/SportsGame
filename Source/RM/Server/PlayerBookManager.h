#pragma once

#include "RM/Data/NetworkData.h"
#include "PlayerBookManager.generated.h"

UCLASS(BlueprintType)
class RM_API UPlayerBookManager : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCollectionResponseEvent, const FPlayerUserResponseData&, ResponseData);

public:
	UFUNCTION(BlueprintCallable, Category = PlayerBookManager)
	void RequestPlayerPieceAcquisitiion(const int32 PlayerId);

	UFUNCTION(BlueprintCallable, Category = PlayerBookManager)
	void RequestPlayerAcquisition(const int32 PlayerId);

private:
	const FPlayerUserResponseData GetPlayerUserResponseData(const int32 PlayerId);

public:
	UPROPERTY(BlueprintAssignable, Category = PlayerBookManager)
	FPlayerCollectionResponseEvent OnPlayerPieceAcqusitionResponse;

	UPROPERTY(BlueprintAssignable, Category = PlayerBookManager)
	FPlayerCollectionResponseEvent OnPlayerAcquisitionResponse;
};
