#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCollectionStructs.h"
#include "CUserWidget_PlayerCard.generated.h"

UCLASS()
class RM_API UCUserWidget_PlayerCard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		class UImage* Image_Card;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Card_Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Card_UniformNumber;

public:
	void SetCardMaterial(UMaterialInstanceDynamic* Material);

public:
	void UpdateCard(FCCollectionPlayerInfo CollectionPlayerInfo, bool bGrayScale);

protected:
	void NativeOnInitialized()override;

private:
	class UMaterialInstanceDynamic* CardMaterial;
};
