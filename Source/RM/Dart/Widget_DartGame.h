// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DartBoard.h"
#include "Widget_DartGame.generated.h"

/**
 * 버튼 클릭시 스폰 (스폰만 해주고)
 * 버튼 떼면 shoot (슛 명령만 내려주고)
 */

UENUM(BlueprintType)
enum class ESTATE_DARTGAME_WIDGET : uint8
{
	None,
	StartGame, CreateDartPin, ShootReady, Shoot, ShootFin,
	RoundEnd, Bust, 
	EndGame, ResumeGame, PausedGame, 
	DeniedShoot
};

UENUM(BlueprintType)
enum class ETYPE_DARTGAME_OVERLAY : uint8
{
	StartGame, HowToPlay, InGame, Paused_PopUp, MiddleResult, End_PopUp
};

UENUM(BlueprintType)
enum class ETYPE_DARTGAME_MODE : uint8
{
	MODE_501, MODE_FULLSCORED
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSetPositionDelegate, FVector, ChangedPosition, bool, bTeleportPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSaveUserDataDelegate, int, score);

UCLASS()
class RM_API UWidget_DartGame : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTimeSecond) override;

	void BindInfo(class ADartBoard* DartBoard);
	void ResetWidget();

	UFUNCTION(BlueprintCallable)
		void SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY overlayType);



public:
	UPROPERTY(BlueprintAssignable)
		FVoidDelVoidDYN OnExitBtnClickedDelegate;
	UPROPERTY(BlueprintAssignable)
		FSetPositionDelegate OnDartChangedPositionDelegate;
	UPROPERTY(BlueprintAssignable)
		FVoidDelVoidDYN OnBackToLobbyDelegate;
	UPROPERTY(BlueprintAssignable)
		FSaveUserDataDelegate OnSaveDataDelegate;

	// Widget Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UserWidgetClass")
		TSubclassOf<class UWidget_DartScored> scoredWidgetClass;

protected:


	UFUNCTION(BlueprintCallable)
		void UpdateCurrentMousePosition(const FPointerEvent& InMouseOrTouchEvent);
	UFUNCTION(BlueprintCallable)
		void GetCurrentMousePositionToWorldSpawnLocation(FVector& WorldPos);
	
	UFUNCTION()
		void AnimationFinished_InfoTextAnim();
	UFUNCTION(BlueprintNativeEvent)
		void BackToLobby();
	virtual void BackToLobby_Implementation();

	// Override Function For Input Touch
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)   override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)   override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual bool CheckCanShootDart(const FPointerEvent& InPointerEvent);
private:
	// Btn Action Function
	// StartGame Overlay Button Bind Fucntion
	UFUNCTION(BlueprintCallable)
		void ClickedStartBtn();
	// InGame Overlay Button Bind Function
	UFUNCTION(BlueprintCallable)
		void OnClickedPausedBtn();
	// PausedPopUp Button Bind Function
	UFUNCTION()
		void ClickedPausedPopup_MoveBack();
	UFUNCTION()
		void ClickedPausedPopup_Continue();
	UFUNCTION()
		void ClickedPausedPopup_NewGame();

	// update
	void UpdateDartPinLocation(bool bTeleportPosition);
	void UpdateRemainRoundTime();

	// state
	void ChangeState(ESTATE_DARTGAME_WIDGET s);
	void StateProcess();

	UFUNCTION()
		void RoundEndAction();
	UFUNCTION()
		void BustAction();
	UFUNCTION()
		void EndGameAction();

	// else Binding Function
	UFUNCTION(BlueprintCallable)
		void RetryGame();
	UFUNCTION()
		void EndMainCamViewBlended();
	UFUNCTION()
		void DetachRoundEndInfo();

	UFUNCTION()
		void PlayScoredWidgetAnim(int score);

	UFUNCTION()
		void GetScored(int score);


	// Starting Fucntion
	UFUNCTION()
		void SetDefaultWidget();
	UFUNCTION()
		void SetDefaultBinding();

private:
	// 21.10.28 Widget 재공사로 인한 추가 변수
	// Widget BP 변수들
	
	// Overlays
	/*
		Index
		0) Start
		1) Info
		2) InGame
		3) Pause_Popup
		4) Middle Result Popup
		5) End Popup
	*/
	UPROPERTY()
		TArray<class UOverlay*> Overlays;

	// Overlay01 (StartGame)
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UButton* startButton01;
private:
	UPROPERTY()
		class UButton* exitButton01;

	// Overlay02 (Info)
	UPROPERTY()
		class UButton* startButton02;
	
	// Overlay03 (Ingame)
	UPROPERTY()
		class UBorder* dartControlBorder;
	UPROPERTY()
		class UButton* dartControlButton;
	UPROPERTY()
		class UButton* pausedButton;
	UPROPERTY()
		class UTextBlock* remainTimeText;
	UPROPERTY()
		class UTextBlock* infoText;


	// Overlay04 (Pause_popup)
	UPROPERTY()
		class UButton* Paused_MoveBackButton;
	UPROPERTY()
		class UButton* Paused_NewGameButton;
	UPROPERTY()
		class UButton* Paused_ContinueButton;

	// Overlay05 (Middle Result)
	UPROPERTY()
		class UButton* MiddleResult_NextRoundButton;
	UPROPERTY()
		class UTextBlock* MiddleResult_RoundNumber;
	UPROPERTY()
		class UTextBlock* MiddleResult_RoundEntireScore;


	// Overlay06 (End)
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UButton* EndPopup_PlayAgainButton;
	// UPROPERTY()
	// 	class UButton* EndPopup_MoveLobbyButton;
private:
	UPROPERTY()
		class UOverlay* EndPopup_SucceededOverlay;
	UPROPERTY()
		class UOverlay* EndPopup_FailedOverlay;
	UPROPERTY()
		class UButton* exitButton02;

	// State
	UPROPERTY(VisibleInstanceOnly)
		ESTATE_DARTGAME_WIDGET state = ESTATE_DARTGAME_WIDGET::None;
	UPROPERTY(VisibleInstanceOnly)
		ETYPE_DARTGAME_MODE dartGameMode = ETYPE_DARTGAME_MODE::MODE_501;

	// BG Image
	UPROPERTY()
		class UImage* BackGroundIMG;

	// Anim
	UPROPERTY(Meta = (BindWidgetAnim), Meta = (AllowPrivateAccess = true), Transient)
		UWidgetAnimation* InfoTextAnim;
	UPROPERTY()
		FWidgetAnimationDynamicEvent InfoTextAnim_EndDel;
	
	// Widget
	UPROPERTY()
		class UWidget_DartScored* scoredWidget;
	// Overlay03 InGame
	UPROPERTY()
		class UWidget_DartGameVerticalRoundInfo* verticalRoundInfoWidget;
	UPROPERTY()
		class UWidget_DartBoard* dartBoardWidget;
	// Overlay05 Middle Result Overlay
	UPROPERTY()
		class UWidget_Dart_BottomPan* bottomRoundInfoWidget;

	


	// Dart Actor Ptr
	UPROPERTY()
		TWeakObjectPtr<AActor> controllingDartPin;
	UPROPERTY()
		ADartBoard* dartBoard;

	// FVector
	FVector2D currentMousePos;
	FVector startDartPinPos;
	FVector endDartPinPos;

	// deltaTime
	float DefaultRemainRoundTime;
	float remainRoundTime;

	// left Scored & Round
	static const int DefaultLeftScore;
	int leftScore = 501;
	int tempLeftScore = 0;
	int roundNum = 1;
	const int lastRoundNum = 10;

	// PressedTimeDelta
	static const float MaximumPressedTime;
	static const float MinimumPressedTime;
	float PressedTimeDelta = 0.0f;

	// TimerHandle
	FTimerHandle RoundManagerTimerHnd;

	// All RoundScore Info
	TArray<int> roundScoreInfo;

	// TargetViewBlendedTime
	static const float targetViewBlendedTime;

	// Flag CleardGame?
	bool bIsFirstStart = true;
	bool bIsClearGame = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool IsTutorial = false;

	UPROPERTY()
	bool IsTutorial1 = false;

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeResultButton();
};


/*
	Widget Btn Hovered
	일정 Tick이 지나면 Start Position 간격마다 업데이트
	
	만약 Button Release되면 Dart Shoot
	-> Start Vector와 Release되었을때의 Mouse Position에서 벡터값을 받아서 날려주자


*/