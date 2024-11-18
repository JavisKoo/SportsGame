// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Kismet/KismetTextLibrary.h"
#include "RM/MiniGame/MiniGameSubsystem.h"
#include "RM/RM_Singleton.h"
#include "CPP_Freekick_Left_Pan.h"
#include "PaperSprite.h"
#include "DBTweenWidget.h"
#include "RMGameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "FreeKickMain.generated.h"

UENUM(BlueprintType)
enum EWidgetState
{
	StartGame, HowToPlay, InGame, Paused, MiddleResult, End
};

UENUM(BlueprintType)
enum EInGameState
{
	CountDown, Play, Pause
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayAgain);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoNotShowTutorial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameInfo, int, Score);

UCLASS()
class RM_API UFreeKickMain : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* Sound_Whistle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* Sound_GameEnd;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:	// Function
	UFUNCTION()
		void SetupCastWidget();
	UFUNCTION()
		void SetupBinding();
	UFUNCTION()
		void Reset();
	UFUNCTION(BlueprintCallable)
		void ChangeWidgetState(EWidgetState State);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsPlay();
	UFUNCTION(BlueprintCallable)
		void RefreshCountDown();
	UFUNCTION(BlueprintCallable)
		void RefreshRound();
	UFUNCTION(BlueprintCallable)
		void RefreshScore();
	UFUNCTION(BlueprintCallable)
		void RefreshRoundStart();
	UFUNCTION(BlueprintCallable)
		void RefreshTime();
	UFUNCTION(BlueprintCallable)
		void SetEnablePausedButton(bool bPauseButtonEnabled);
	
	UFUNCTION(BlueprintCallable)
		void FreeKickMainFadeEvent(EScreenFaderEvent Fader);
	
	UFUNCTION(BlueprintCallable)
		void OnClickedStart();
	UFUNCTION(BlueprintCallable)
		void OnClickedExit();
	UFUNCTION(BlueprintCallable)
		void OnClickedPause();
	UFUNCTION(BlueprintCallable)
		void OnClickedResume();
	UFUNCTION()
		void OnClickedNextRound();
	UFUNCTION(BlueprintCallable)
		void OnClickedPlayAgain();
	UFUNCTION()
		void OnClickedNewGame();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EWidgetState GetWidgetState() { return WidgetState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EInGameState GetIngameState() { return InGameState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EInGameState GetPrevInGameState() { return PrevInGameState; }

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeResultButton();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnStartGame OnStartGame;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnExit OnExit;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnNextRound OnNextRound;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnPlayAgain OnPlayAgain;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnTimeout OnTimeout;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnSaveGameInfo OnSaveGameInfo;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnNewGame OnNewGame;
	UPROPERTY()
		FNotifyGameViewportChanged FadeViewportHandle;

private:	// Variable
	EWidgetState WidgetState;
	bool bFirstStart = true;

	EInGameState PrevInGameState;
	EInGameState InGameState;

	const float DefaultRemainStageTime = 10.0f;
	float RemainStageTime;

	UPROPERTY(EditAnywhere)
	TArray<UPaperSprite*> CountDownSprites;
	FTimerHandle CountDownTimer;
	FTimerHandle CountDownAniTimer;
	FTimerHandle FadeScreenTimer;
	
	const float CountDownInterval = 1.0f;
	const float CountDownDelay = 0.25f;
	int CountDownIndex;

	UPROPERTY(meta=(bindWidget))
	UImage* ImgDimmed;

	UPROPERTY(meta=(bindWidget))
	UImage* ImgCountDown;
	
	UPROPERTY()
	TArray<UOverlay*> Overlays;

	UPROPERTY(meta=(bindWidget))
		UButton* BtnExit;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(bindWidget))
		UButton* BtnStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(bindWidget))
		UButton* BtnPlayAgain;
private:
	UPROPERTY(meta=(bindWidget))
		UButton* BtnStartFromTuto;
	UPROPERTY(meta=(bindWidget))
		UButton* BtnPause;
	UPROPERTY(meta=(bindWidget))
		UButton* BtnExitFromPause;
	UPROPERTY(meta=(bindWidget))
		UButton* BtnNewGame;
	UPROPERTY(meta=(bindWidget))
		UButton* BtnContinue;
	UPROPERTY(meta=(bindWidget))
		UButton* BtnNextRound;
	UPROPERTY(meta=(bindWidget))
		UButton* BtnExitFromEnd;

	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtStartGame;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtRank;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtExit;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtStartFromTuto;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtRoundText1;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtRound1;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtStage1;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtScoreText1;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtScore1;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtRemainText1;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtRemain1;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtSecText;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtMiddleRoundText;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtMiddleRound;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtMiddleRoundScore;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtTotalText;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtTotalResult;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtRankFromEnd;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtExitFromEnd;
	UPROPERTY(meta=(bindWidget))
		UTextBlock* TxtPlayAgain;

	UPROPERTY(meta=(bindWidget))
		UCPP_Freekick_Left_Pan* Freekick_Left_Pan;

	UPROPERTY(meta=(bindWidget))
		UCPP_Freekick_Left_Pan* Freekick_Left_Pan_1;
};
//avatarcustomizing