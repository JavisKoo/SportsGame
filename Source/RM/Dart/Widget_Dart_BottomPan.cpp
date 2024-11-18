// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Dart_BottomPan.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"


void UWidget_Dart_BottomPan::NativeConstruct()
{
	Super::NativeConstruct();

	HorizontalBox_Score = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HorizontalBox_Score")));
}

void UWidget_Dart_BottomPan::SetScoreTextBoxes(TArray<int> scoreValues)
{
	TArray<UWidget*> HB_ScoreArr = HorizontalBox_Score->GetAllChildren();

	for (int i = 0; i < HB_ScoreArr.Num(); i++)
	{
		if (!scoreValues.IsValidIndex(i))
			break;

		UTextBlock* child = Cast<UTextBlock>(HB_ScoreArr[i]);
		if (child != nullptr)
		{
			child->SetText(FText::FromString(FString::FromInt(scoreValues[i])));
		}
	}
}

void UWidget_Dart_BottomPan::SetScoreTextBoxInIndex(int index, int scoreValue)
{
	TArray<UWidget*> HB_ScoreArr = HorizontalBox_Score->GetAllChildren();
	if (!HB_ScoreArr.IsValidIndex(index))
	{
		UE_LOG(LogSlate, Warning, TEXT("HorizontalBox_Score has not child in %d index"), index);
		return;
	}

	UTextBlock* child = Cast<UTextBlock>(HB_ScoreArr[index]);
	if (child == nullptr)
	{
		UE_LOG(LogSlate, Warning, TEXT("HorizontalBox_Score has not child in %d index for EditableBox"), index);
		return;
	}
	if (scoreValue >= 0)
		child->SetText(FText::FromString(FString::FromInt(scoreValue)));
	else
		child->SetText(FText::FromString(TEXT("")));
}

void UWidget_Dart_BottomPan::SetLeftScoreText(int lastRoundIndex, int leftScore)
{
	TArray<UWidget*> HB_ScoreArr = HorizontalBox_Score->GetAllChildren();
	if (!HB_ScoreArr.IsValidIndex(lastRoundIndex))
	{
		UE_LOG(LogSlate, Warning, TEXT("HorizontalBox_Score has not child in %d index"), lastRoundIndex);
		return;
	}

	UTextBlock* child = Cast<UTextBlock>(HB_ScoreArr[lastRoundIndex]);
	child->SetText(FText::FromString(FString::FromInt(leftScore)));
}

void UWidget_Dart_BottomPan::ResetScoreTextBoxes(int DefualtLeftScore)
{
	TArray<UWidget*> HB_ScoreArr = HorizontalBox_Score->GetAllChildren();
	for (int i = 0; i < HB_ScoreArr.Num() - 1; i++)
		SetScoreTextBoxInIndex(i, -1);

	SetLeftScoreText(HB_ScoreArr.Num() - 1, DefualtLeftScore);
}
