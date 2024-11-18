#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSavePlayer.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FCardIndexData
{
		GENERATED_BODY()
public:
		FCardIndexData(){}

		UPROPERTY(BlueprintReadWrite)
		TArray<FName> PlayerList;
};

UCLASS()
class RM_API ACSavePlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSavePlayer();

public:
	UFUNCTION(BlueprintCallable)
	void SavePlayerCards(FName UserNickname, FName CardRowName);

	UFUNCTION(BlueprintCallable)
	TArray<FName> LoadPlayerCardsArray(FName UserNickname);

protected:
	virtual void BeginPlay() override;
};
