// Fill out your copyright notice in the Description page of Project Settings.


#include "RMGameViewportClient.h"
#include "Engine/Canvas.h"

void URMGameViewportClient::PostRender(UCanvas* Canvas)
{
	Super::PostRender(Canvas);
	const UWorld* world = GetWorld();
	if (World) {
		AccTime += World->GetDeltaSeconds();
	}
	DrawScreenFade(Canvas);
}

void URMGameViewportClient::DrawScreenFade(UCanvas* Canvas)
{
	if (Fader != EScreenFader::ENone) {
		if (World) {
			FLinearColor Color = FadeColor;
			switch (Fader) {
			case EScreenFader::EFadeIn:
				FadeColor.A = FMath::GetMappedRangeValueClamped(FVector2D(0, Duration), FVector2D(0, 1), AccTime);
				if (FMath::IsNearlyEqual(Color.A, 1.0f)) {
					GameViewportDelegate.ExecuteIfBound(EScreenFaderEvent::FadeInEnded);
					Fader = EScreenFader::EFill;
				}

				break;
			case EScreenFader::EFadeOut:
				FadeColor.A = FMath::GetMappedRangeValueClamped(FVector2D(0, Duration), FVector2D(1, 0), AccTime);
				if (FMath::IsNearlyEqual(Color.A, 0.0f)) {
					GameViewportDelegate.ExecuteIfBound(EScreenFaderEvent::FadeOutEnded);
					Fader = EScreenFader::ENone;
				}
				break;

			}
			
			FColor OldColor = Canvas->DrawColor;
			Canvas->DrawColor = Color.ToFColor(true);
			Canvas->DrawTile(Canvas->DefaultTexture, 0, 0, Canvas->ClipX, Canvas->ClipY, 0, 0, Canvas->DefaultTexture->GetSizeX(), Canvas->DefaultTexture->GetSizeY());
			Canvas->DrawColor = OldColor;
		}
	}
}

void URMGameViewportClient::ClearFade()
{
	Fader = EScreenFader::ENone;
}

void URMGameViewportClient::Fill(FLinearColor Color)
{
	const UWorld* world = GetWorld();
	if (world)
	{
		Fader = EScreenFader::EFill;
		FadeColor = Color;
		Duration = 0;
		AccTime = 0;
	}
}

void URMGameViewportClient::FadeIn(const float InDuration, FLinearColor InColor,
	const FNotifyGameViewportChanged& OnNotifyGameViewportChanged)
{
	const UWorld* world = GetWorld();
	if (world)
	{
		GameViewportDelegate = OnNotifyGameViewportChanged;
		GameViewportDelegate.ExecuteIfBound(EScreenFaderEvent::FadeInStart);
		Fader = EScreenFader::EFadeIn;
		FadeColor = InColor;
		FadeColor.A = 0.0f;
		Duration = InDuration;
		AccTime = 0;
	}
}

void URMGameViewportClient::FadeOut(const float InDuration, FLinearColor InColor,
	const FNotifyGameViewportChanged& OnNotifyGameViewportChanged)
{
	const UWorld* world = GetWorld();
	if (world)
	{
		GameViewportDelegate = OnNotifyGameViewportChanged;
		GameViewportDelegate.ExecuteIfBound(EScreenFaderEvent::FadeOutStart);
		Fader = EScreenFader::EFadeOut;
		FadeColor = InColor;
		FadeColor.A = 1.0f;
		Duration = InDuration;
		AccTime = 0;
	}
}
