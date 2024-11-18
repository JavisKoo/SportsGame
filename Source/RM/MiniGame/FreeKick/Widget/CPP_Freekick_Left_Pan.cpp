// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Freekick_Left_Pan.h"
#define LOCTEXT_NAMESPACE "TextFormatActions"

void UCPP_Freekick_Left_Pan::NativeConstruct()
{
	Super::NativeConstruct();

	auto widgets = VB_Round->GetAllChildren();
	for (int i = 0; i < widgets.Num(); i++)
	{
		auto textBlock = Cast<UTextBlock>(widgets[i]);
		if (textBlock != nullptr)
		{
			FText OutText;
			if (i == widgets.Num() -1)
			{
				textBlock->SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("119_Total"), OutText) ? OutText : OutText);
			}
			else
			{
				textBlock->SetText(FText::Format(LOCTEXT("RoundTextFormat", "{0}{1}"),
				FText::AsNumber(i + 1),
				FText::FindText(TEXT("41_MiniGame"), TEXT("111_Round"), OutText) ? OutText : OutText));
			}
		}
	}
}

void UCPP_Freekick_Left_Pan::RefreshWidget()
{
	auto widgets = VB_Score->GetAllChildren();
	for (int i = 0; i < widgets.Num(); i++)
	{
		auto textBlock = Cast<UTextBlock>(widgets[i]);
		if (textBlock != nullptr)
		{
			if (i == widgets.Num() - 1)
			{
				textBlock->SetText(FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->GetTotalScore()));
			}
			else
			{
				textBlock->SetText(i <= GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->RoundIndex
				? FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->GetRoundScore(i))
				: FText::FromString(""));
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE