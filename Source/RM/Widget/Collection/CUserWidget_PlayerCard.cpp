#include "CUserWidget_PlayerCard.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UCUserWidget_PlayerCard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UCUserWidget_PlayerCard::SetCardMaterial(UMaterialInstanceDynamic* Material)
{
	CardMaterial = Material;
}

void UCUserWidget_PlayerCard::UpdateCard(FCCollectionPlayerInfo CollectionPlayerInfo, bool bGrayScale)
{
	//SetBrush
	if (!CollectionPlayerInfo.PlayerCardImage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, "PlayerCardImage is NULL");
		return;
	}

	CardMaterial->SetTextureParameterValue("Texture", CollectionPlayerInfo.PlayerCardImage);
	if (bGrayScale)
	{
		CardMaterial->SetScalarParameterValue("GrayScale", 1.0f);
	}
	else
	{
		CardMaterial->SetScalarParameterValue("GrayScale", 0.0f);
	}
	Image_Card->SetBrushFromMaterial(CardMaterial);

	//SetName
	TextBlock_Card_Name->SetText(FText::FromString(CollectionPlayerInfo.Name));

	//SetUniformNumber
	TextBlock_Card_UniformNumber->SetText(FText::FromString(FString::FromInt(CollectionPlayerInfo.UniformNumber)));
}