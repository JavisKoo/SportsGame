// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeKickMain.h"

#include "DBTweenFloat.h"
#include "DBTweenUpdateManager.h"
#include "RM/RMBlueprintFunctionLibrary.h"

const FString TweenCountDownScale = FString("CountDownScale");
const FString TweenCountDownFade = FString("TweenCountDownFade");

void UFreeKickMain::NativeConstruct()
{
	Super::NativeConstruct();

	SetupCastWidget();
	SetupBinding();
	ChangeWidgetState(StartGame);

}

void UFreeKickMain::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsPlay())
		RefreshTime();
}

void UFreeKickMain::SetupCastWidget()
{
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("1_Start"))));
	//get utc time
	// FString utcTime = FDateTime::UtcNow().ToString();
	
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("2_Tutorial"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("3_InGame"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("4_Paused"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("5_Middle_Result"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("6_End"))));

#define FindText FindText
	FText OutText;
	TxtStartGame->		SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("001_start_game"), OutText) ? OutText : OutText);
	TxtStartFromTuto->	SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("001_start_game"), OutText) ? OutText : OutText);
	TxtRank->			SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("006_RANKING"), OutText) ? OutText : OutText);
	TxtRankFromEnd->	SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("006_RANKING"), OutText) ? OutText : OutText);
	TxtRoundText1->		SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("111_Round"), OutText) ? OutText : OutText);
	TxtScoreText1->		SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("118_Score"), OutText) ? OutText : OutText);
	TxtRemainText1->	SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("105_Remain"), OutText) ? OutText : OutText);
	TxtSecText->		SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("106_Sec"), OutText) ? OutText : OutText);
	TxtMiddleRoundText->SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("111_Round"), OutText) ? OutText : OutText);
	TxtTotalText->		SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("119_Total"), OutText) ? OutText : OutText);
	TxtExit->			SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("120_Exit"), OutText) ? OutText : OutText);
	TxtExitFromEnd->	SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("120_Exit"), OutText) ? OutText : OutText);
	TxtPlayAgain->		SetText(FText::FindText(TEXT("41_MiniGame"), TEXT("004_play_again"), OutText) ? OutText : OutText);	
}

void UFreeKickMain::SetupBinding()
{
	//if (IsValid(BtnStart))
	//	BtnStart->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedStart);
	
	if (IsValid(BtnStartFromTuto))
		BtnStartFromTuto->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedStart);
	
	//if (IsValid(BtnExit))
	//	BtnExit->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedExit);
	
	if (IsValid(BtnPause))
		BtnPause->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedPause);
	
	//if (IsValid(BtnExitFromPause))
	//	BtnExitFromPause->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedExit);

	/* Modified - OnClickedStart() -> OnClickedNewGame() */
	if (IsValid(BtnNewGame))
		BtnNewGame->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedNewGame);

	if (IsValid(BtnContinue))
		BtnContinue->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedResume);

	if (IsValid(BtnNextRound))
		BtnNextRound->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedNextRound);
	
	//if (IsValid(BtnPlayAgain))
	//	BtnPlayAgain->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedPlayAgain);

	//if (IsValid(BtnExitFromEnd))
	//	BtnExitFromEnd->OnClicked.AddDynamic(this, &UFreeKickMain::OnClickedExit);

	FadeViewportHandle.BindDynamic(this, &UFreeKickMain::FreeKickMainFadeEvent);
}

/* 2022.04.26
 * Added by Junho
 * @Desc: 카운트 다운 관련 버그 발생으로 타이머 검사 후, Clear && StartGame으로 Widget을 다시 시작합니다.
 */
void UFreeKickMain::Reset()
{
	const UWorld* World = GetWorld();
	if (World->GetTimerManager().IsTimerActive(CountDownTimer)
		|| World->GetTimerManager().IsTimerPaused(CountDownTimer))
			World->GetTimerManager().ClearTimer(CountDownTimer);
	if (World->GetTimerManager().IsTimerActive(CountDownAniTimer)
		|| World->GetTimerManager().IsTimerPaused(CountDownAniTimer))
			World->GetTimerManager().ClearTimer(CountDownAniTimer);

	BtnPause->SetIsEnabled(true);
	ChangeWidgetState(EWidgetState::StartGame);
}

void UFreeKickMain::ChangeWidgetState(EWidgetState State)
{
	if (WidgetState != State)
	{
		uint8 index = (uint8)State;
		
		// 메인 백그라운드 이미지 On/Off
		ImgDimmed->SetVisibility(State == InGame ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

		// 사용할 패널 보여주기
		for (int i = 0; i < Overlays.Num(); i++)
			Overlays[i]->SetVisibility(index == i ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	WidgetState = State;
	switch (State)
	{
		case StartGame:
			InGameState = CountDown;
			if (bFirstStart == false)
			{
				ChangeWidgetState(InGame);
				return;
			}
			break;
		case HowToPlay:break;
		case InGame:
			if (InGameState == CountDown)
			{
				/* 2022.04.26 */
				/* Modify by Junho
				 * @Desc: Game Pause 후 ChangeState할 시, 카운트가 3초부터 다시 세어지는 버그 때문에 Timer Activate를 검사했습니다.
				 */
				const UWorld* World = GetWorld();
				if (World && World->GetTimerManager().IsTimerPaused(CountDownTimer))
					World->GetTimerManager().UnPauseTimer(CountDownTimer);
				if (World && World->GetTimerManager().IsTimerPaused(CountDownAniTimer))
					World->GetTimerManager().UnPauseTimer(CountDownAniTimer);
				
				if (World && !World->GetTimerManager().IsTimerActive(CountDownTimer))
				{
					ImgCountDown->SetVisibility(ESlateVisibility::Hidden);
					CountDownIndex = CountDownSprites.Num();
					if (CountDownIndex > 0)
						World->GetTimerManager().SetTimer(CountDownTimer, this, &UFreeKickMain::RefreshCountDown, CountDownInterval, true);
					else
						UFreeKickMain::RefreshCountDown();
				}
				/* 2022.04.26 */
				/* 이전 함수 Body */
				/*
					if (const UWorld* World = GetWorld())
					{
						ImgCountDown->SetVisibility(ESlateVisibility::Hidden);
						CountDownIndex = CountDownSprites.Num();
						if (CountDownIndex > 0)
							World->GetTimerManager().SetTimer(CountDownTimer, this, &UFreeKickMain::RefreshCountDown, CountDownInterval, true);
						else
							UFreeKickMain::RefreshCountDown();
					}
				 */
			}
			break;
		case Paused:

			/* 2022.04.26 */
			/* Modify by Junho
			 * @Desc: Game Puase 시, 안정성을 위해 CountDownTimer, CountDownAniTimer가 Activate되어 있다면, Pause 시켰습니다.
			 */
			if (const UWorld* World = GetWorld())
			{
				if (World->GetTimerManager().IsTimerActive(CountDownTimer))
					World->GetTimerManager().PauseTimer(CountDownTimer);
				if (World->GetTimerManager().IsTimerActive(CountDownAniTimer))
					World->GetTimerManager().PauseTimer((CountDownAniTimer));
			}
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			break;
		
		case MiddleResult:
			if (GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->CheckFinalRound())
			{
				ChangeWidgetState(End);
			}
			else
			{
				TxtMiddleRound->SetText(FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->RoundIndex + 1));
				TxtMiddleRoundScore->SetText(FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->GetRoundScore()));
				Freekick_Left_Pan->RefreshWidget();
			}
			break;
		
		case End:
			TxtTotalResult->SetText(FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->GetTotalScore()));
			Freekick_Left_Pan_1->RefreshWidget();

			//Play GameEnd Sound.
			if(Sound_GameEnd)
				UGameplayStatics::PlaySound2D(this, Sound_GameEnd);

			if (OnSaveGameInfo.IsBound())
				OnSaveGameInfo.Broadcast(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->GetTotalScore());

			ChangeResultButton();
			break;
		default: ;
	}
}

bool UFreeKickMain::IsPlay()
{
	return (WidgetState == InGame && InGameState == Play  && RemainStageTime >= 0);
}

void UFreeKickMain::RefreshCountDown()
{
	UDBTweenWidget::DBTweenStop(TweenCountDownScale);
	UDBTweenWidget::DBTweenStop(TweenCountDownFade);
	if (CountDownIndex <= 0 )
	{
		if (const UWorld* World = GetWorld())
			World->GetTimerManager().ClearTimer(CountDownTimer);
		ImgCountDown->SetVisibility(ESlateVisibility::Hidden);
		InGameState = Play;
	}
	else
	{
		if (ImgCountDown->Visibility != ESlateVisibility::SelfHitTestInvisible)
			ImgCountDown->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		ImgCountDown->SetBrushFromAtlasInterface(CountDownSprites[CountDownIndex - 1], true);
		ImgCountDown->SetRenderScale(FVector2D(1.5f, 1.5f));
		ImgCountDown->SetRenderOpacity(0.0f);

		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CountDownAniTimer);
			
			const float delayTime = CountDownIndex == 1 ? CountDownDelay * 2 : CountDownDelay;
			const float durTime = CountDownInterval - delayTime;
			World->GetTimerManager().SetTimer(CountDownAniTimer, FTimerDelegate::CreateLambda([=]()
			{
				//Play Sound Whistle.
				if(CountDownIndex == 1 && Sound_Whistle)
					UGameplayStatics::PlaySound2D(this, Sound_Whistle);
				
				UDBTweenWidget::DOLocalScale(TweenCountDownScale, durTime, ImgCountDown, FVector2D(1.0f, 1.0f),
					CountDownIndex == 1 ? EaseType::OutElastic : EaseType::OutCubic);
				
				UDBTweenWidget::DOFade(TweenCountDownFade, durTime, ImgCountDown, 1.0f,
					CountDownIndex == 1 ? EaseType::OutElastic : EaseType::OutCubic);

				CountDownIndex--;
			}), delayTime, false);
		}
	}
}

void UFreeKickMain::RefreshRound()
{
	RefreshScore();
	RefreshRoundStart();

	RemainStageTime = DefaultRemainStageTime + 1;
	RefreshTime();
}

void UFreeKickMain::RefreshScore()
{
	TxtScore1->SetText(FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->GetRoundScore()));
}

void UFreeKickMain::RefreshRoundStart()
{
	TxtRound1->SetText(FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->RoundIndex + 1));
	TxtStage1->SetText(FText::AsNumber(GetGameInstance()->GetSubsystem<UMiniGameSubsystem>()->StageIndex + 1));
}

void UFreeKickMain::RefreshTime()
{
	if (RemainStageTime >= 0)
	{
		float time = FMath::Max(RemainStageTime, 0.0f);
		FTimespan timeSpan = FTimespan::FromSeconds(time);
		FString TimeStr = (timeSpan.GetSeconds() >= 11)
				   ? FString::FromInt(timeSpan.GetSeconds() - 1) : FString::FromInt(timeSpan.GetSeconds());
		
		TxtRemain1->SetText(FText::FromString(FString::Printf(TEXT("00:%s"), *TimeStr)));

		RemainStageTime -= GetWorld()->DeltaTimeSeconds;
	
		if (RemainStageTime < 0)
		{
			if (OnTimeout.IsBound())
				OnTimeout.Broadcast();
		}
	}
}

/* 2022.04.26 */ 
/* @Added by Junho
 * @Desc: Screen이 Fade Action일 때, Game이 Pause되지 않도록 BP에서 컨트롤할 수 있는 BP함수입니다.
 */
void UFreeKickMain::SetEnablePausedButton(bool bPauseButtonEnabled)
{
	BtnPause->SetIsEnabled(bPauseButtonEnabled);
}

/* 2022.04.26 */ 
/* @Added by Junho
 * @Desc: Game Pause후, NewGame Btn 클릭시, Fade이벤트를 발생, 해당 이벤트에서 OnPlayAgain Delegate를 FadeIn되었을때 실행하기 위한 함수힙니다.
 */
void UFreeKickMain::FreeKickMainFadeEvent(EScreenFaderEvent Fader)
{
	switch(Fader)
	{
		case EScreenFaderEvent::FadeInStart:
			BtnPause->SetIsEnabled(false);
			Overlays[3]->SetVisibility(ESlateVisibility::HitTestInvisible);
			UE_LOG(LogTemp, Log, TEXT("FadeInStarted"));
			break;
		case EScreenFaderEvent::FadeInEnded:
			{
				static const float timerRate = 0.5f;
				GetWorld()->GetTimerManager().SetTimer(FadeScreenTimer, FTimerDelegate::CreateLambda([this]()
				{
					Reset();
					const float DurationTime = 0.25f;
					URMBlueprintFunctionLibrary::ScreenFadeOut(GetWorld(), DurationTime, FColor::Black, FadeViewportHandle);
				}), timerRate, false);
			
				if (OnPlayAgain.IsBound())
					OnPlayAgain.Broadcast();
				/* GamePlay Pause를 False처리 해줘야만 등록해둔 Timer가 돌아갑니다. */
				UGameplayStatics::SetGamePaused(GetWorld(), false);
				UE_LOG(LogTemp, Log, TEXT("FadeInEnded"));
				break;
			}
		case EScreenFaderEvent::FadeOutStart:
			UE_LOG(LogTemp, Log, TEXT("FadeOutStart"));
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::Printf(TEXT("FadeOutStart")));
			break;
		case EScreenFaderEvent::FadeOutEnded:
			FadeViewportHandle.Clear();
			UE_LOG(LogTemp, Log, TEXT("FadeOutEnded"));
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::Printf(TEXT("FadeOutEnded")));
			break;
	}
}

#pragma region OnClickEvent

void UFreeKickMain::OnClickedStart()
{
	/*  Reset Function needed. */
	Reset();
	
	if (bFirstStart)
	{
		if (Cast<UOverlay>(GetWidgetFromName(TEXT("2_Tutorial")))->GetVisibility()==ESlateVisibility::Collapsed)
		{
			bFirstStart = false;
			InGameState = CountDown;
			ChangeWidgetState(InGame);
		
			if (OnStartGame.IsBound())
				OnStartGame.Broadcast();
		}
		else
		{
			ChangeWidgetState(HowToPlay);
			bFirstStart = false;
		}
	}
	else
	{
		InGameState = CountDown;
		ChangeWidgetState(InGame);
		
		if (OnStartGame.IsBound())
			OnStartGame.Broadcast();
	}
}

void UFreeKickMain::OnClickedExit()
{
	if (const UWorld* World = GetWorld())
	{
		if (UGameplayStatics::IsGamePaused(World))
			UGameplayStatics::SetGamePaused(World, false);
	}
	
	if (OnExit.IsBound())
		OnExit.Broadcast();
}

void UFreeKickMain::OnClickedPause()
{
	PrevInGameState = InGameState;
	InGameState = Pause;
	ChangeWidgetState(Paused);
}

void UFreeKickMain::OnClickedResume()
{
	InGameState = PrevInGameState;
	ChangeWidgetState(InGame);
}

void UFreeKickMain::OnClickedNextRound()
{
	InGameState = CountDown;
	ChangeWidgetState(InGame);

	if (OnNextRound.IsBound())
		OnNextRound.Broadcast();
}

void UFreeKickMain::OnClickedPlayAgain()
{
	WidgetState = StartGame;
	
	ChangeWidgetState(StartGame);
	
	if (OnPlayAgain.IsBound())
		OnPlayAgain.Broadcast();
}

/* 2022.04.26
 * Modify by Junho
 * @Desc: NewGame Button 클릭시, Screen Fade를 추가하기 위한 새로운 바인딩 함수입니다.
 */
void UFreeKickMain::OnClickedNewGame()
{
	if (!FadeViewportHandle.IsBound())
		FadeViewportHandle.BindDynamic(this, &UFreeKickMain::FreeKickMainFadeEvent);
	
	URMBlueprintFunctionLibrary::ScreenFadeIn(GetWorld(), 0.25f, FColor::Black, FadeViewportHandle);
}
#pragma endregion 
