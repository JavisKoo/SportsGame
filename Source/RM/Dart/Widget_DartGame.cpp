// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_DartGame.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/Image.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "Widget_DartScored.h"
#include "Widget_DartBoard.h"
#include "Widget_DartGameVerticalRoundInfo.h"
#include "Widget_Dart_BottomPan.h"
#include "Datasmith/DatasmithCore/Public/DatasmithDefinitions.h"
#include "Internationalization/StringTable.h"

const int UWidget_DartGame::DefaultLeftScore = 501;
const float UWidget_DartGame::MaximumPressedTime = 3.0f;
const float UWidget_DartGame::MinimumPressedTime = 0.15f;
const float UWidget_DartGame::targetViewBlendedTime = 1.0f;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		PUBLIC FUNCTIONS		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UWidget_DartGame::NativeConstruct()
{
	Super::NativeConstruct();

	SetDefaultWidget();
	SetDefaultBinding();
	SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::StartGame);

	FText stringKey = FText::FromStringTable("/Game/RM/Localization/41_MiniGame.41_MiniGame", "001_start_game");
	if (IsValid(GEngine))
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, stringKey.ToString());
}

void UWidget_DartGame::NativeTick(const FGeometry& MyGeometry, float DeltaTimeSecond)
{
	Super::NativeTick(MyGeometry, DeltaTimeSecond);
	StateProcess();
}

/// <summary>
/// �־��� DartBoard ������ ������ �����ͷ� �ް� ��������Ʈ�� �����մϴ�.
/// </summary>
/// <param name="DartBoard">������ ������ ���� ��Ʈ���� ���� ������</param>
void UWidget_DartGame::BindInfo(ADartBoard* DartBoard)
{
	if (DartBoard == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DartBoard ptr is Nullptr."));
		return;
	}
	dartBoard = DartBoard;
	dartBoard->onScoredDelegate.AddDynamic(this, &UWidget_DartGame::GetScored);

	DefaultRemainRoundTime = dartBoard->DefaultRoundTime;
#if PLATFORM_WINDOWS || UE_EDITOR
	/*FInputModeUIOnly gameMode;
	GetWorld()->GetFirstPlayerController()->SetInputMode(gameMode);*/
#endif
}


/// <summary>
/// this ������ ������ �ʱⰪ���� �����ϴ�. 
/// </summary>
void UWidget_DartGame::ResetWidget()
{
	// Reset Game or Game Start.
	controllingDartPin = nullptr;
	bIsClearGame = true;
	
	leftScore = tempLeftScore = (dartGameMode == ETYPE_DARTGAME_MODE::MODE_501) ? DefaultLeftScore : 0;
	
	remainRoundTime = DefaultRemainRoundTime;
	roundScoreInfo.Empty(0);
	roundNum = 1;

	dartControlBorder->SetIsEnabled(true);
	pausedButton->SetIsEnabled(true);
	SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::InGame);

	if (dartBoard != nullptr)
	{
		dartBoard->ResetRound();
	}

	if (dartBoardWidget != nullptr)
	{
		dartBoardWidget->SetScoreLeftText(leftScore);
		dartBoardWidget->ResetAllScoreBoxText();
	}

	if (verticalRoundInfoWidget != nullptr)
	{
		verticalRoundInfoWidget->ResetAllRoundScore_VB();
		verticalRoundInfoWidget->SetScoreLeftText(lastRoundNum, leftScore);
	}

	if (bottomRoundInfoWidget != nullptr)
		bottomRoundInfoWidget->ResetScoreTextBoxes(DefaultLeftScore);
}


/// <summary>
/// index�� �ش��ϴ� Overlay�� Visible��Ű�� �������� Hidden ��ŵ�ϴ�.
/// </summary>
/// <param name="index">Overlays�� index��</param>
void UWidget_DartGame::SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY overlayType)
{
	uint8 index = (uint8)overlayType;

	if (index >= Overlays.Num())
		index = Overlays.Num() - 1;

	if (overlayType == ETYPE_DARTGAME_OVERLAY::InGame)
		BackGroundIMG->SetVisibility(ESlateVisibility::Hidden);
	else
		BackGroundIMG->SetVisibility(ESlateVisibility::Visible);

	for (int i = 0; i < Overlays.Num(); i++)
	{
		if (index == i)
			Overlays[i]->SetVisibility(ESlateVisibility::Visible);
		else
			Overlays[i]->SetVisibility(ESlateVisibility::Hidden);
	}

	if((uint8)overlayType == 5)
		ChangeResultButton();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		PROPTECTED FUNCTIONS		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Update CurrentMousePos, if occured controlBorder Event. 
/// </summary>
/// <param name="InMouseOrTouchEvent">Event Mouse or Touch</param>
void UWidget_DartGame::UpdateCurrentMousePosition(const FPointerEvent& InMouseOrTouchEvent)
{
	currentMousePos = InMouseOrTouchEvent.GetScreenSpacePosition();

	FVector2D PixelPos, ViewportPos;
	USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(), currentMousePos, PixelPos, ViewportPos);
	currentMousePos = PixelPos;

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	if (currentMousePos.X > ViewportSize.X)
		currentMousePos.X = ViewportSize.X;
	else if (currentMousePos.X < 0)
		currentMousePos.X = 0;

	if (currentMousePos.Y > ViewportSize.Y)
		currentMousePos.Y = ViewportSize.Y;
	else if (currentMousePos.Y < 0)
		currentMousePos.Y = 0;

	//UE_LOG(LogSlate, Warning, TEXT("Current Mouse Pos : %s"), *currentMousePos.ToString());
}


/// <summary>
/// Viewport�� Ŀ�� �������� �������� lay�� ���� ���� ����� �ݸ����� WorldLocation�� �˾Ƴ��ϴ�.
/// </summary>
/// <param name="WorldPos">���Ϲ��� WorldPos Ref</param>
void UWidget_DartGame::GetCurrentMousePositionToWorldSpawnLocation(FVector& WorldPos)
{
	auto playerController = GetWorld()->GetFirstPlayerController();
	FHitResult hitresult;
	if (playerController->GetHitResultAtScreenPosition(currentMousePos, ECollisionChannel::ECC_GameTraceChannel1, false, hitresult))
	{
		WorldPos = hitresult.ImpactPoint;
		return;
	}
}

void UWidget_DartGame::AnimationFinished_InfoTextAnim()
{
	// ���� ViewTarget�� LeftSide
	// Delay�� 1�� �ִٰ� ���ƿ���
	// dartBoardDelegate�� BlendTargetView�� ������ ���� ����
	GetWorld()->GetTimerManager().SetTimer(RoundManagerTimerHnd, FTimerDelegate::CreateLambda([this]()
		{	// Start LambdaEndMainCamViewBlended
			dartBoard->onEndOfBlendViewTargetDelegate.AddDynamic(this, &UWidget_DartGame::EndMainCamViewBlended);
			dartBoard->SetBlendTargetMainCam(1.0f);
		}), 0.01f, false, 1.0f);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		OVERRIDE TOUCH INPUT FUCNTION		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FReply UWidget_DartGame::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	FReply reply = Super::NativeOnTouchStarted(InGeometry, InGestureEvent);

	uint8 index = (uint8)ETYPE_DARTGAME_OVERLAY::InGame;
	bool isOnInGameOverlay = Overlays[index]->IsVisible();
	auto playerController = GetWorld()->GetFirstPlayerController();

	if (!dartControlBorder->GetIsEnabled() || !isOnInGameOverlay ||
		dartBoard == nullptr || playerController == nullptr || 
		state == ESTATE_DARTGAME_WIDGET::ShootReady)
		return reply;

	PressedTimeDelta = 0.0f;
	UpdateCurrentMousePosition(InGestureEvent);

	if (!controllingDartPin.IsValid())
		ChangeState(ESTATE_DARTGAME_WIDGET::CreateDartPin);
	else
	{
		UpdateDartPinLocation(true);
		ChangeState(ESTATE_DARTGAME_WIDGET::ShootReady);
	}
	return reply;
}

FReply UWidget_DartGame::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	FReply reply = Super::NativeOnTouchMoved(InGeometry, InGestureEvent);

	uint8 index = (uint8)ETYPE_DARTGAME_OVERLAY::InGame;
	bool isOnInGameOverlay = Overlays[index]->IsVisible();
	if (isOnInGameOverlay)
		UpdateCurrentMousePosition(InGestureEvent);

	return reply;
}

FReply UWidget_DartGame::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	FReply reply = Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
	
	uint8 index = (uint8)ETYPE_DARTGAME_OVERLAY::InGame;
	bool isOnInGameOverlay = Overlays[index]->IsVisible();
	if (!dartControlBorder->GetIsEnabled() || controllingDartPin == nullptr || !isOnInGameOverlay || state == ESTATE_DARTGAME_WIDGET::Shoot)
		return reply;

	if (CheckCanShootDart(InGestureEvent))
		ChangeState(ESTATE_DARTGAME_WIDGET::Shoot);
	else
		ChangeState(ESTATE_DARTGAME_WIDGET::DeniedShoot);

	return reply;
}

void UWidget_DartGame::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	uint8 index = (uint8)ETYPE_DARTGAME_OVERLAY::InGame;
	bool isOnInGameOverlay = Overlays[index]->IsVisible();
	if (!dartControlBorder->GetIsEnabled() || controllingDartPin == nullptr || !isOnInGameOverlay || state == ESTATE_DARTGAME_WIDGET::Shoot) return;

	if (CheckCanShootDart(InMouseEvent))
		ChangeState(ESTATE_DARTGAME_WIDGET::Shoot);
	else
		ChangeState(ESTATE_DARTGAME_WIDGET::DeniedShoot);
}

bool UWidget_DartGame::CheckCanShootDart(const FPointerEvent& InPointerEvent)
{
	UpdateCurrentMousePosition(InPointerEvent);
	FVector WorldPos;
	GetCurrentMousePositionToWorldSpawnLocation(WorldPos);
	endDartPinPos = WorldPos;
	FVector direction = endDartPinPos - startDartPinPos;

	if (direction.Z < 0 || direction.Size() < 1.0f ||
		PressedTimeDelta > MaximumPressedTime || PressedTimeDelta <= MinimumPressedTime)
		return false;
	else
		return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		PRIVATE FUNCTIONS		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		BUTTON ACTION FUNCTIONS		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UWidget_DartGame::ClickedStartBtn()
{
	if (bIsFirstStart)
	{
		if (IsTutorial==false)
		{
			SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::HowToPlay);
			SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::InGame);
			ChangeState(ESTATE_DARTGAME_WIDGET::StartGame);
			bIsFirstStart = false;
		}
		else
		{
			SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::HowToPlay);
			bIsFirstStart = false;	
		}
	}
	else
	{
		SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::InGame);
		ChangeState(ESTATE_DARTGAME_WIDGET::StartGame);
	}
}
// Override Function For Input Touch
void UWidget_DartGame::OnClickedPausedBtn()
{
	ChangeState(ESTATE_DARTGAME_WIDGET::PausedGame);
}

void UWidget_DartGame::ClickedPausedPopup_MoveBack()
{
	BackToLobby();
}

void UWidget_DartGame::ClickedPausedPopup_Continue()
{
	auto playerController = GetWorld()->GetFirstPlayerController();
	if (playerController != nullptr)
	{
		playerController->SetPause(false);
	}

	ChangeState(ESTATE_DARTGAME_WIDGET::DeniedShoot);
}
  
void UWidget_DartGame::ClickedPausedPopup_NewGame()
{
	auto playerController = GetWorld()->GetFirstPlayerController();
	if (playerController != nullptr)
	{
		playerController->SetPause(false);
	}

	ChangeState(ESTATE_DARTGAME_WIDGET::StartGame);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		UPDATE & STATE FUNCTIONS		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UWidget_DartGame::UpdateDartPinLocation(bool bTeleportPosition)
{
	if (!controllingDartPin.IsValid()) return;

	FVector impactPoint;
	GetCurrentMousePositionToWorldSpawnLocation(impactPoint);
	if (OnDartChangedPositionDelegate.IsBound())
		OnDartChangedPositionDelegate.Broadcast(impactPoint, bTeleportPosition);

	startDartPinPos = controllingDartPin->GetActorLocation();
}

void UWidget_DartGame::UpdateRemainRoundTime()
{
	remainRoundTime -= GetWorld()->DeltaTimeSeconds;
	remainTimeText->SetText(FText::FromString(FString::FromInt(FMath::CeilToInt(remainRoundTime))));
	
	if (remainRoundTime < 0)
	{
		// ChangeState
		ChangeState(ESTATE_DARTGAME_WIDGET::DeniedShoot);
		ChangeState(ESTATE_DARTGAME_WIDGET::RoundEnd);
	}
}

void UWidget_DartGame::ChangeState(ESTATE_DARTGAME_WIDGET s)
{
	if (s == state) return;

	state = s;
	switch (state)
	{
		case ESTATE_DARTGAME_WIDGET::StartGame:
			ResetWidget();
			break;
		case ESTATE_DARTGAME_WIDGET::CreateDartPin:
			if (IsValid(dartBoard->GetSpawnedDartPin()))
			{
				controllingDartPin = dartBoard->GetSpawnedDartPin();
			}
			else
			{
				FVector impactPoint;
				GetCurrentMousePositionToWorldSpawnLocation(impactPoint);
				dartBoard->SpawnDartPin(impactPoint, FVector(3.0f));
				controllingDartPin = dartBoard->GetSpawnedDartPin();
			}
			ChangeState(ESTATE_DARTGAME_WIDGET::ShootReady);
			break;
		case ESTATE_DARTGAME_WIDGET::ShootReady:
			pausedButton->SetIsEnabled(true);
			if (controllingDartPin != nullptr)
				controllingDartPin->SetActorHiddenInGame(false);
			break;
		case ESTATE_DARTGAME_WIDGET::Shoot:
			dartControlBorder->SetIsEnabled(false);
			pausedButton->SetIsEnabled(false);
			dartBoard->ShootDartPinWithPressedTime(startDartPinPos, endDartPinPos, PressedTimeDelta);
			UE_LOG(LogTemp, Log, TEXT("PressedTime: %f"), PressedTimeDelta);
			break;
		case ESTATE_DARTGAME_WIDGET::ShootFin:
			controllingDartPin = nullptr;
			dartControlBorder->SetIsEnabled(true);
			pausedButton->SetIsEnabled(true);
			break;
		case ESTATE_DARTGAME_WIDGET::RoundEnd:
			// dartBoard SetBlendViewTarget + Bind Delegate end of blendTarget
			dartControlBorder->SetIsEnabled(false);
			dartBoard->SetBlendTargetRoundEndCam(1.0f);
			dartBoard->onEndOfBlendViewTargetDelegate.AddDynamic(this, &UWidget_DartGame::RoundEndAction);
			break;
		case ESTATE_DARTGAME_WIDGET::Bust:
			// dartBoard SetBlendViewTarget
			dartBoard->SetBlendTargetRoundEndCam(1.0f);
			dartControlBorder->SetIsEnabled(false);
			dartBoard->onEndOfBlendViewTargetDelegate.AddDynamic(this, &UWidget_DartGame::BustAction);
			break;
		case ESTATE_DARTGAME_WIDGET::ResumeGame:
			dartBoard->ResetRound();
			dartControlBorder->SetIsEnabled(true);
			pausedButton->SetIsEnabled(true);
			SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::InGame);
			break;
		case ESTATE_DARTGAME_WIDGET::PausedGame:
			{
				//auto playerController = GetWorld()->GetFirstPlayerController();
				//if (playerController != nullptr)
				//{
				//	playerController->SetPause(true);
				//}
				dartControlBorder->SetIsEnabled(false);
				SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::Paused_PopUp);
			}
			break;
		case ESTATE_DARTGAME_WIDGET::EndGame:
			// UI ����ְ� Delegate ����
			// dartBoard SetBlendViewTarget
			dartBoard->SetBlendTargetRoundEndCam(1.0f);
			dartControlBorder->SetIsEnabled(false);
			dartBoard->onEndOfBlendViewTargetDelegate.AddDynamic(this, &UWidget_DartGame::EndGameAction);
			break;
		case ESTATE_DARTGAME_WIDGET::DeniedShoot:
			if (controllingDartPin != nullptr)
				controllingDartPin->SetActorHiddenInGame(true);
			pausedButton->SetIsEnabled(true);
			SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::InGame);
			dartControlBorder->SetIsEnabled(true);
			if (PressedTimeDelta > MaximumPressedTime)
				UE_LOG(LogTemp, Log, TEXT("PressedTime Exceed MaximumPressedTime!  :: PressedTime: %f"), PressedTimeDelta);

			PressedTimeDelta = 0.0f;
			break;
	}
}

void UWidget_DartGame::StateProcess()
{
	switch (state)
	{
		case ESTATE_DARTGAME_WIDGET::StartGame:
			UpdateRemainRoundTime();
			break;
		case ESTATE_DARTGAME_WIDGET::ShootReady:
			PressedTimeDelta += GetWorld()->DeltaTimeSeconds;
			UpdateDartPinLocation(false);
			UpdateRemainRoundTime();

		#if ENABLE_DRAW_DEBUG && UE_EDITOR
			// ������ �Լ� �߰�
			if (PressedTimeDelta <= 3.0f)
			{
				FVector WorldPos;
				GetCurrentMousePositionToWorldSpawnLocation(WorldPos);
				if (dartBoard != nullptr)
					dartBoard->DrawRandomRange(WorldPos, FColor::Red, 0.1f, PressedTimeDelta);
			}

			if (IsValid(GEngine))
			{
				FString TimeString = TEXT("Pressed Time Delta : ") + FString::SanitizeFloat(PressedTimeDelta);

				if (PressedTimeDelta <= 3.0f)
					GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Blue, TimeString);
				else
					GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Red, TimeString);
			}
		#endif
			break;
		case ESTATE_DARTGAME_WIDGET::Shoot:
			break;
		case ESTATE_DARTGAME_WIDGET::ShootFin:
			UpdateRemainRoundTime();
			break;
		case ESTATE_DARTGAME_WIDGET::RoundEnd:
			break;
		case ESTATE_DARTGAME_WIDGET::ResumeGame:
			UpdateRemainRoundTime();
			break;
		case ESTATE_DARTGAME_WIDGET::DeniedShoot:
			if (controllingDartPin.IsValid())
			{
				FVector WorldLocation;
				GetCurrentMousePositionToWorldSpawnLocation(WorldLocation);
				controllingDartPin->SetActorLocation(WorldLocation);
			}
			UpdateRemainRoundTime();
			break;
	}
}

/// <summary>
/// �� ���尡 ������ �Ǹ� �ߵ�,
/// </summary>
void UWidget_DartGame::RoundEndAction()
{
	if (controllingDartPin != nullptr)
		controllingDartPin = nullptr;

	if (dartBoardWidget != nullptr)
	{
		dartBoardWidget->ResetAllScoreBoxText();
	}

	// Score ����
	int currentRoundScore = FMath::Abs<int>(leftScore - tempLeftScore);
	leftScore = tempLeftScore;
	roundScoreInfo.Add(currentRoundScore);
	infoText->SetText(FText::FromString(FString::FromInt(currentRoundScore)));

	// ���� ��Ƽ�� ���� �������� ����
	if (verticalRoundInfoWidget != nullptr)
	{
		verticalRoundInfoWidget->SetRoundScoreByIndex_VB(roundNum - 1, currentRoundScore);
		verticalRoundInfoWidget->SetScoreLeftText(lastRoundNum, leftScore);
	}

	// Round ����
	remainRoundTime = DefaultRemainRoundTime;
	roundNum++;

	// Round�� 10Round�� �Ѿ��� ����� ó��
	if (roundNum > lastRoundNum)
	{
		bIsClearGame = (dartGameMode == ETYPE_DARTGAME_MODE::MODE_501) ? false : true;
		state = ESTATE_DARTGAME_WIDGET::EndGame;
	}
	
	PlayAnimation(InfoTextAnim);
}

void UWidget_DartGame::BustAction()
{
	if (controllingDartPin != nullptr)
		controllingDartPin = nullptr;

	if (dartBoardWidget != nullptr)
	{
		dartBoardWidget->SetScoreLeftText(leftScore);
		dartBoardWidget->ResetAllScoreBoxText();
	}

	tempLeftScore = leftScore;
	roundScoreInfo.Add(0);
	infoText->SetText(FText::FromString(TEXT("Bust!")));

	if (verticalRoundInfoWidget != nullptr)
	{
		verticalRoundInfoWidget->SetRoundScoreByIndex_VB(roundNum - 1, 0);
		verticalRoundInfoWidget->SetScoreLeftText(lastRoundNum, leftScore);
	}

	remainRoundTime = DefaultRemainRoundTime;
	roundNum++;

	//���尡 10Round�� �Ѿ����� ó��
	if (roundNum > lastRoundNum)
	{
		bIsClearGame = false;
		state = ESTATE_DARTGAME_WIDGET::EndGame;
	}
	
	PlayAnimation(InfoTextAnim);
}

void UWidget_DartGame::EndGameAction()
{
	if (controllingDartPin != nullptr)
		controllingDartPin = nullptr;
	
	if (dartBoardWidget != nullptr)
	{
		dartBoardWidget->SetScoreLeftText(0);
	}
	int currentRoundScore = FMath::Abs<int>(leftScore - tempLeftScore);
	leftScore = tempLeftScore;
	roundScoreInfo.Add(currentRoundScore);
	infoText->SetText(FText::FromString(FString::FromInt(currentRoundScore)));

	// ���� ��Ƽ�� ���� �������� ����
	if (verticalRoundInfoWidget != nullptr)
	{
		verticalRoundInfoWidget->SetRoundScoreByIndex_VB(roundNum - 1, currentRoundScore);
		verticalRoundInfoWidget->SetScoreLeftText(lastRoundNum, leftScore);
	}

	// Round ����
	remainRoundTime = DefaultRemainRoundTime;
	//roundNum++;

	PlayAnimation(InfoTextAnim);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		BINDING FUNCTIONS		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UWidget_DartGame::RetryGame()
{
	ChangeState(ESTATE_DARTGAME_WIDGET::StartGame);
}

void UWidget_DartGame::EndMainCamViewBlended()
{
	// 2. ���� ���� & �Լ�����
	if (state == ESTATE_DARTGAME_WIDGET::EndGame)
	{
		int saveScore;
		if (dartGameMode == ETYPE_DARTGAME_MODE::MODE_501)
		{
			saveScore = roundNum * 10;
			saveScore = FMath::Clamp<int>(saveScore, 10, lastRoundNum * 10);
			saveScore += (dartBoard->GetCurrentDartpinIndex()) ? dartBoard->GetCurrentDartpinIndex() : 3;
			saveScore = (bIsClearGame) ? saveScore : 104;	// ���� ���н� ���� ����ó��
		}
		else if (dartGameMode == ETYPE_DARTGAME_MODE::MODE_FULLSCORED)
		{
			saveScore = leftScore;
		}
		
		if (bIsClearGame)
		{
			EndPopup_SucceededOverlay->SetVisibility(ESlateVisibility::Visible);
			EndPopup_FailedOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			EndPopup_SucceededOverlay->SetVisibility(ESlateVisibility::Hidden);
			EndPopup_FailedOverlay->SetVisibility(ESlateVisibility::Visible);
		}

		// ������ ������ ���̺긦 ���� ���� ����.
		if (OnSaveDataDelegate.IsBound() && IsValid(dartBoard))
		{
			UE_LOG(LogTemp, Warning, TEXT("Saved Score: %d"), saveScore);
			OnSaveDataDelegate.Broadcast(saveScore);
		}
		SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::End_PopUp);
		return;
	}
	else
	{
		if (bottomRoundInfoWidget != nullptr)
		{
			bottomRoundInfoWidget->SetLeftScoreText(lastRoundNum, leftScore);
			for (int i = 0; i < roundScoreInfo.Num(); i++)
				bottomRoundInfoWidget->SetScoreTextBoxInIndex(i, roundScoreInfo[i]);
		}
		MiddleResult_RoundNumber->SetText(FText::FromString(FString::FromInt(roundNum - 1)));
		MiddleResult_RoundEntireScore->SetText(FText::FromString(FString::FromInt(roundScoreInfo.Top())));
		SetOverlayVisibility(ETYPE_DARTGAME_OVERLAY::MiddleResult);
	}
}

void UWidget_DartGame::DetachRoundEndInfo()
{
	ChangeState(ESTATE_DARTGAME_WIDGET::ResumeGame);
}

void UWidget_DartGame::BackToLobby_Implementation()
{
	auto playerController = GetWorld()->GetFirstPlayerController();
	playerController->SetPause(false);
	
	if (OnBackToLobbyDelegate.IsBound())
		OnBackToLobbyDelegate.Broadcast();

	dartBoard->ExitFromDartGame();
	//RemoveFromViewport();
}

void UWidget_DartGame::PlayScoredWidgetAnim(int score)
{
	scoredWidget = CreateWidget<UWidget_DartScored>(GetWorld(), scoredWidgetClass);
	scoredWidget->AddToViewport(0);
	scoredWidget->SetWidgetLocation(controllingDartPin->GetActorLocation());
	scoredWidget->SetScoreTextAndPlayAnim(score);
}

void UWidget_DartGame::GetScored(int score)
{
	if (scoredWidgetClass == nullptr)
	{
		UE_LOG(LogSlate, Error, TEXT("ScoredWidgetClass == nullptr! \n Can't Create ScoredWidget"));
		return;
	}

	// �Ͻ������� ����� ���� ǥ�� ���� ����
	PlayScoredWidgetAnim(score);

	int index = dartBoard->GetCurrentDartpinIndex();
	if (dartBoardWidget != nullptr)
	{	
		if (index == 0)
			index = 3;

		if (dartGameMode == ETYPE_DARTGAME_MODE::MODE_501)
		{
			tempLeftScore -= score;
			// Bustó��
			if (tempLeftScore < 0)
			{
				// Bust Widget ����ְ�
				// Create Lambda Change WidgetState
				dartControlBorder->SetIsEnabled(false);
				GetWorld()->GetTimerManager().SetTimer(RoundManagerTimerHnd, FTimerDelegate::CreateLambda([this]()
					{	// Start Lambda
						ChangeState(ESTATE_DARTGAME_WIDGET::Bust);
					}), // End Lambda
					1.0f, false, 0.5f);
				return;
			}
			// EndGame ó��
			else if (tempLeftScore == 0)
			{
				dartBoardWidget->SetScoreBoxText(index, score);
				dartControlBorder->SetIsEnabled(false);
				// Create Lambda Change WidgetState	
				GetWorld()->GetTimerManager().SetTimer(RoundManagerTimerHnd, FTimerDelegate::CreateLambda([this]()
					{	// Start Lambda
						ChangeState(ESTATE_DARTGAME_WIDGET::EndGame);
					}), // End Lambda
					1.0f, false, 1.0f);
				return;
			}
		}
		else if (dartGameMode == ETYPE_DARTGAME_MODE::MODE_FULLSCORED)
		{
			tempLeftScore += score;
		}

		// �� ������ ���� ����
		dartBoardWidget->SetScoreBoxText(index, score);
		dartBoardWidget->SetScoreLeftText(tempLeftScore);
	}

	if (verticalRoundInfoWidget != nullptr)
	{
		verticalRoundInfoWidget->SetScoreLeftText(lastRoundNum, tempLeftScore);
	}

	if (index < 3)
		ChangeState(ESTATE_DARTGAME_WIDGET::ShootFin);
	else
	{
		// Round End ó��
		controllingDartPin = nullptr;
		dartControlBorder->SetIsEnabled(false);
		GetWorld()->GetTimerManager().SetTimer(RoundManagerTimerHnd, FTimerDelegate::CreateLambda([this]()
			{	// Start Lambda
				ChangeState(ESTATE_DARTGAME_WIDGET::RoundEnd);
			}), // End Lambda
			1.0f, false, 1.0f);
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		STARTING FUNCTIONS		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UWidget_DartGame::SetDefaultWidget()
{
	// Get Variable In Derived BlueprintWidgetClass
	// Get Main Overlays And Set
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("OVL1_Start"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("OVL2_Info"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("OVL3_InGame"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("OVL4_Paused_PopUp"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("OVL5_Middle_Result"))));
	Overlays.Add(Cast<UOverlay>(GetWidgetFromName(TEXT("OVL6_End_PopUp"))));

	// Get Start Overlay (Overlays[0])
	startButton01 = Cast<UButton>(GetWidgetFromName(TEXT("BTN_StartGame01")));
	exitButton01 = Cast<UButton>(GetWidgetFromName(TEXT("BTN_EXIT")));

	// Get Info Overlay (Overlays[1])
	startButton02 = Cast<UButton>(GetWidgetFromName(TEXT("BTN_StartGame02")));

	// Get InGame Overlay (Overlays[2])
	dartControlBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BORDER_PinControl")));
	dartControlButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_PinControl")));
	pausedButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_PAUSED")));
	infoText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_INFO")));
	remainTimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_RemainTimeNum")));
	dartBoardWidget = Cast<UWidget_DartBoard>(GetWidgetFromName(TEXT("WidgetBP_ScoreBoard")));
	verticalRoundInfoWidget = Cast<UWidget_DartGameVerticalRoundInfo>(GetWidgetFromName(TEXT("WidgetBP_LeftVerticalRoundInfo")));

	// Get Paused Popup Overlay (Overlays[3])
	Paused_MoveBackButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_MoveArea")));
	Paused_NewGameButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_NewGame")));
	Paused_ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_Continue")));

	// Get MiddleResult Popup Overlay (Overlays[4])
	MiddleResult_NextRoundButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_NextRound")));
	MiddleResult_RoundEntireScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_RoundEntireScore")));
	MiddleResult_RoundNumber = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Middle_RoundNumber")));
	bottomRoundInfoWidget = Cast<UWidget_Dart_BottomPan>(GetWidgetFromName(TEXT("WidgetBP_Dart_Bottom_Pan")));

	// Get End Popup Overlay (Overlays[5])
	EndPopup_PlayAgainButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_Play_Again")));
	// EndPopup_MoveLobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("BTN_Move_Lobby")));
	EndPopup_SucceededOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("OVL_Result_Success")));
	EndPopup_FailedOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("OVL_Result_Fail")));
	exitButton02 = Cast<UButton>(GetWidgetFromName(TEXT("BTN_EXIT_1")));

	// Image
	BackGroundIMG = Cast<UImage>(GetWidgetFromName(TEXT("IMG_Dimm_Start_End")));
}

void UWidget_DartGame::SetDefaultBinding()
{
	// Bind Delegate 
	// 1) Start & Info Overlay Binding
	//if (startButton01 != nullptr)
	//	startButton01->OnClicked.AddDynamic(this, &UWidget_DartGame::ClickedStartBtn);
	if (startButton02 != nullptr)
		startButton02->OnClicked.AddDynamic(this, &UWidget_DartGame::ClickedStartBtn);
	//if (exitButton01 != nullptr)
	//	exitButton01->OnClicked.AddDynamic(this, &UWidget_DartGame::BackToLobby);

	// 2) InGame Overlay Widget Binding
	if (pausedButton != nullptr)
		pausedButton->OnClicked.AddDynamic(this, &UWidget_DartGame::OnClickedPausedBtn);
	
	// 3) MiddleResult Overlay Widget Binding
	if (MiddleResult_NextRoundButton != nullptr)
		MiddleResult_NextRoundButton->OnClicked.AddDynamic(this, &UWidget_DartGame::DetachRoundEndInfo);

	// 4) Paused Popup Overlay Widget Binding
	if (Paused_NewGameButton != nullptr)
		Paused_NewGameButton->OnClicked.AddDynamic(this, &UWidget_DartGame::ClickedPausedPopup_NewGame);
	//if (Paused_MoveBackButton != nullptr)
	//	Paused_MoveBackButton->OnClicked.AddDynamic(this, &UWidget_DartGame::ClickedPausedPopup_MoveBack);
	if (Paused_ContinueButton != nullptr)
		Paused_ContinueButton->OnClicked.AddDynamic(this, &UWidget_DartGame::ClickedPausedPopup_Continue);

	// 5) End Popup Overlay Widget Binding
	//if (EndPopup_PlayAgainButton != nullptr)
	//	EndPopup_PlayAgainButton->OnClicked.AddDynamic(this, &UWidget_DartGame::RetryGame);
	// if (EndPopup_MoveLobbyButton != nullptr)
	// 	EndPopup_MoveLobbyButton->OnClicked.AddDynamic(this, &UWidget_DartGame::BackToLobby);
	//if (exitButton02 != nullptr)
	//	exitButton02->OnClicked.AddDynamic(this, &UWidget_DartGame::BackToLobby);


	// Bind AnimFinsihed Delegate
	InfoTextAnim_EndDel.BindDynamic(this, &UWidget_DartGame::AnimationFinished_InfoTextAnim);
	BindToAnimationFinished(InfoTextAnim, InfoTextAnim_EndDel);

}