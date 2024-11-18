// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RMDefinedEnum.h"
#include "Engine/GameViewportClient.h"
#include "RMGameViewportClient.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FNotifyGameViewportChanged, EScreenFaderEvent, Fader);

UCLASS()
class RM_API URMGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

protected:
	virtual void PostRender(UCanvas* Canvas) override;
	void DrawScreenFade(UCanvas* Canvas);

public:
	void ClearFade();
	void Fill(FLinearColor Color);

	void FadeIn(const float Duration, FLinearColor Color, const FNotifyGameViewportChanged& OnNotifyGameViewportChanged);
	void FadeOut(const float Duration, FLinearColor Color, const FNotifyGameViewportChanged& OnNotifyGameViewportChanged);

private:
	EScreenFader Fader = EScreenFader::ENone;
	float AccTime = 0.0f;
	float Duration;
	FLinearColor FadeColor;
	FNotifyGameViewportChanged GameViewportDelegate;
};
