// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_DartBoard.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"

void UWidget_DartBoard::NativeConstruct()
{
	Super::NativeConstruct();
	Score_01Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Score_01")));
	Score_02Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Score_02")));
	Score_03Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Score_03")));
	Score_leftText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_LeftScore")));

	for (int i = 0; i < 3; i++)
	{
		FString name = TEXT("IMG_DartPin") + FString::FromInt(i);
		UImage* img = Cast<UImage>(GetWidgetFromName(*name));
		if (img != nullptr)
			IMG_DartImages.Add(img);
	}
}

void UWidget_DartBoard::SetScoreBoxText(int index, int score)
{
	index--;

	if (IMG_DartImages.IsValidIndex(index))
		IMG_DartImages[index]->SetVisibility(ESlateVisibility::Hidden);

	if (Score_01Text && Score_02Text && Score_03Text)
	{
		FText ConvertScore = FText::FromString(FString::FromInt(score));
		switch (index)
		{
		case 0:
			if (Score_01Text != nullptr)
			{
				if (score > 0)
					Score_01Text->SetText(ConvertScore);
				else
					Score_01Text->SetText(FText::FromString(TEXT("miss")));
			}
			break;
		case 1:
			if (Score_02Text != nullptr)
			{
				if (score > 0)
					Score_02Text->SetText(ConvertScore);
				else
					Score_02Text->SetText(FText::FromString(TEXT("miss")));
			}
			break;
		case 2:
			if (Score_03Text != nullptr)
			{
				if (score > 0)
					Score_03Text->SetText(ConvertScore);
				else
					Score_03Text->SetText(FText::FromString(TEXT("miss")));
			}
			break;
		}
		return;
	}
}

void UWidget_DartBoard::SetScoreLeftText(int leftScore)
{
	if (Score_leftText != nullptr)
	{
		Score_leftText->SetText(FText::FromString(FString::FromInt(leftScore)));
		return;
	}
}

int UWidget_DartBoard::GetLeftScore()
{
	if (Score_leftText != nullptr)
	{
		FString num = Score_leftText->GetText().ToString();
		int number = FCString::Atoi(*num);
		return number;
	}
	else
	{
		UE_LOG(LogSlate, Warning, TEXT("Can't Find UWidget_DartBoard::Score_leftText! , Return 0Value Now."));
		return 0;
	}
		
}

void UWidget_DartBoard::ResetAllScoreBoxText()
{
	for (UImage* img : IMG_DartImages)
		img->SetVisibility(ESlateVisibility::Visible);

	if (Score_leftText != nullptr && Score_01Text && Score_02Text && Score_03Text)
	{
		Score_01Text->SetText(FText::FromString(TEXT("")));
		Score_02Text->SetText(FText::FromString(TEXT("")));
		Score_03Text->SetText(FText::FromString(TEXT("")));
		return;
	}
}