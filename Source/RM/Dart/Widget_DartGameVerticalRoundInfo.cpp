// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_DartGameVerticalRoundInfo.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"

void UWidget_DartGameVerticalRoundInfo::NativeConstruct()
{
	Super::NativeConstruct();
	VB_RoundScoreBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("VB_RoundScore")));
}

void UWidget_DartGameVerticalRoundInfo::ResetAllRoundScore_VB()
{
	auto children = VB_RoundScoreBox->GetAllChildren();
	for (auto child : children)
	{
		auto castingChild = Cast<UTextBlock>(child);
		if (castingChild != nullptr)
		{
			castingChild->SetText(FText::FromString(TEXT("")));
		}
	}
}

void UWidget_DartGameVerticalRoundInfo::SetAllRoundScore_VB(const TArray<int>& scores)
{
	if (VB_RoundScoreBox == nullptr)
	{
		UE_LOG(LogSlate, Warning, TEXT("VB_RoundScoreBox is nullptr!"));
		return;
	}

	auto children = VB_RoundScoreBox->GetAllChildren();

	for (int i = 0; i < scores.Num(); i++)
	{
		if (!children.IsValidIndex(i))
			return;

		auto castingChild = Cast<UTextBlock>(children[i]);
		if (castingChild != nullptr)
		{
			castingChild->SetText(FText::FromString(FString::FromInt(scores[i])));
		}
	}
}

void UWidget_DartGameVerticalRoundInfo::SetRoundScoreByIndex_VB(const int index, const int value)
{
	if (VB_RoundScoreBox == nullptr)
	{
		UE_LOG(LogSlate, Warning, TEXT("VB_RoundScoreBox is nullptr!"));
		return;
	}

	auto children = VB_RoundScoreBox->GetAllChildren();
	if (!children.IsValidIndex(index))
		return;
	else
	{
		auto castingChild = Cast<UTextBlock>(children[index]);
		if (castingChild != nullptr)
			castingChild->SetText(FText::FromString(FString::FromInt(value)));
	}
}

void UWidget_DartGameVerticalRoundInfo::SetScoreLeftText(const int lastRoundIndex, const int leftScore)
{
	if (VB_RoundScoreBox == nullptr)
	{
		UE_LOG(LogSlate, Warning, TEXT("VB_RoundScoreBox is nullptr!"));
		return;
	}

	auto children = VB_RoundScoreBox->GetAllChildren();
	if (!children.IsValidIndex(lastRoundIndex))
		return;

	auto VB_LeftScore_ET = Cast<UTextBlock>(children[lastRoundIndex]);
	VB_LeftScore_ET->SetText(FText::FromString(FString::FromInt(leftScore)));
}