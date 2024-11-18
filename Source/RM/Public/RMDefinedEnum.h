#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "RMDefinedEnum.generated.h"

UCLASS()
class RM_API URMDefinedEnum : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};

UENUM(BlueprintType)
enum class EGenderRM : uint8
{
	Male,
	Female,
	Male2,
	Female2,
	Male3,
	Female3,
	Male4,
	Female4,
};

UENUM(BlueprintType)
enum class EMorphGroup : uint8
{
	Head,
	Face,
	Body,
	Hand,
	Eye,
	All
};

UENUM(BlueprintType)
enum class EMorphTarget : uint8
{
	EyeScale,
	EyeWide,
	EyeLidShapeA,
	Termple,
	BrowUpdown,
	Cheek,
	Chin,
	ChinTip,
	NoseHigh,
	LipThic,
	LipWide,
	NoseShapeA,
	NoseTip
};

UENUM(BlueprintType)
enum class ECostumePart : uint8
{
	Head,
	Hair,
	Top,
	Bottom,
	Shoes,
	FaceSkin,
	EyeBrow,
	Lip,
	G_Hair,
};

UENUM(BlueprintType)
enum class EEyeMaterialScalarParameter : uint8
{
	EyeColor,
	IrisBrightness,
	PupilScale,
	ScleraBrightness
};

//Priority Based Budgeting
UENUM(BlueprintType)
enum class EPBR : uint8
{
	TopMost,
	High,
	Normal,
	Low,
	Lowest
};

UENUM(BlueprintType)
enum class ERandType : uint8
{
	MorpherHead,
	MorpherFace,
	MorpherBody,
	MorpherHand,
	MorpherEye,
	Head,
	Body,
	FaceSkin,
	SkinColor,
	Costume,
	All
};


UENUM(BlueprintType)
enum class ENewLevelType : uint8
{
	Tour_,
	Tour_Baskball,
	Tour_BestClub,
	Tour_Dressing,
	Tour_Francisco,
	Tour_KingsOfEurope,
	Tour_PhotoMontages,
	Tour_Press,
	Tour_Sensations,

	RealCity_Living,
	RealCity_Player,
	RealCity_RealMadrid,

	NewsRoom,

	VIP_House,
	VIP_Mansion,
	VIP_PoolHouse,
	VIP_VIP
};



UENUM(BlueprintType)
enum class EMapType : uint8 // RoomType
{
	None,
	Tour,
	VIP,
	Normal		UMETA(ToolTip = "NewsRoom"),
	Lobby,
	Community
};

UENUM(BlueprintType)
enum class EScreenFaderEvent : uint8
{
	FadeInStart,
	FadeInEnded,
	FadeOutStart,
	FadeOutEnded
};

UENUM(BlueprintType)
enum class EScreenFader : uint8
{
	ENone,
	EFill,
	EFadeIn,
	EFadeOut,
};

UENUM(BlueprintType)
enum class EStringTable : uint8
{
	None,

	E41_MiniGame,
};

UENUM(BlueprintType)
enum class ENFTActorState : uint8
{
	ENull				 UMETA(DisplayName = "Null"),
	EDownloading		 UMETA(DisplayName = "Downloading"),
	ELooping			 UMETA(DisplayName = "Looping")
};

UENUM(BlueprintType)
enum class EGameTickMode : uint8
{
	Enter,
	Tick,
	Leave,
	MoveForward,
	MoveRight,
	Turn,
	TurnRate,
	LookUp,
	Wheel,
	Emote,
	SwitchView,
	Sit,
	Stand
};



