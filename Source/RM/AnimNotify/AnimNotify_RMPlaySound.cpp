// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_RMPlaySound.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Animation/AnimSequenceBase.h"
#include "../BaseRMGameInstance.h"
#include "../Public/Character/RMPlayerAnimInstance.h"
#include "../Public/Character/RMCharacter.h"
#include "../Public/Character/RMGamePlayerCharacter.h"
UAnimNotify_RMPlaySound::UAnimNotify_RMPlaySound()
	: Super()
{
	
}

void UAnimNotify_RMPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	static bool isInGame = false;
	if (Sound)
	{
		if (Sound->IsLooping())
		{
			UE_LOG(LogAudio, Warning, TEXT("PlaySound notify: Anim %s tried to spawn infinitely looping sound asset %s. Spawning suppressed."), *GetNameSafe(Animation), *GetNameSafe(Sound));
			return;
		}
		isInGame = (MeshComp->GetWorld() != nullptr && MeshComp->GetWorld()->GetGameInstance() != nullptr) ? true : false;

		/* if In Editor Notify */
		if (!isInGame)
		{
			Super::Notify(MeshComp, Animation);
			return;
		}
		else
		{
			if (auto RMGameInstance = Cast<UBaseRMGameInstance>(MeshComp->GetWorld()->GetGameInstance()))
			{
				bool isSFXSoundOn = RMGameInstance->bIsSFXPlay;
				if (bUseRMCharacter)
				{
					auto playableCharacter = Cast<ARMCharacter>(MeshComp->GetOwner());
					auto RMCharacter = Cast<ARMGamePlayerCharacter>(MeshComp->GetOwner());
					auto animInst = playableCharacter != nullptr ? playableCharacter->GetAnimInst() : RMCharacter->GetAnimInst();
					
					auto soundAttenuation = (animInst != nullptr && bUseAttenuation) ? animInst->GestureSoundAttenuation : nullptr;
					auto soundConcurrency = (animInst != nullptr && bUseConcurrency) ? animInst->GestureSoundConcurrency : nullptr;

					if (bFollow && isSFXSoundOn)
					{
						if (AttachName == NAME_None)
							RMGameInstance->PlaySFXSound3D(Sound, MeshComp->GetComponentLocation(), MeshComp->GetComponentRotation(), VolumeMultiplier, PitchMultiplier, 0.0f, soundAttenuation, soundConcurrency, MeshComp->GetOwner());
						else
							UGameplayStatics::SpawnSoundAttached(Sound, MeshComp, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier, 0.0f, soundAttenuation, soundConcurrency);
					}
					else if (!bFollow && isSFXSoundOn)
						UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), Sound, MeshComp->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
				}
			}
			else
			{
				/* RMGameInstance를 사용하지 않으면 일반 PlaySound 노티파이처럼 콜 */
				Super::Notify(MeshComp, Animation);
				return;
			}
		}
	}
}
