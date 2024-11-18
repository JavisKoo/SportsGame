#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimationURO.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RM_API UAnimationURO : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = URO, meta = (AllowPrivateAccess = "true"))
	TArray<int> FrameRate;

public:
	UAnimationURO();

protected:
	virtual void BeginPlay() override;
};
