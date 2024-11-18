// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_DartScored.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
void UWidget_DartScored::NativeConstruct()
{
	Super::NativeConstruct();

	scoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Score")));
	BlendInScore_EndDel.BindDynamic(this, &UWidget_DartScored::AnimationFinished_BlendInScore);
	BindToAnimationFinished(BlendInScore, BlendInScore_EndDel);
	
}

void UWidget_DartScored::SetScoreTextAndPlayAnim(int score)
{
	if (scoreText == nullptr)
		return;

	scoreText->SetText(FText::FromString(FString::FromInt(score)));
	PlayAnimation(BlendInScore);
}

void UWidget_DartScored::SetWidgetLocation(FVector WorldLocation)
{
	float offsetZ = 2.0f;
	WorldLocation += FVector(0.0f, 0.0f, offsetZ);
	FVector2D screenLocation;
	if (UGameplayStatics::ProjectWorldToScreen(GetPlayerContext().GetPlayerController(), WorldLocation, screenLocation))
	{
		SetPositionInViewport(screenLocation);
	}
}

void UWidget_DartScored::AnimationFinished_BlendInScore()
{
	RemoveFromViewport();
}