#pragma once


#include "CoreMinimal.h"
#include "RMStruct.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FActionPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActionPoint")
	int32 point;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActoinPoint")
	FString updatedAt;
};


USTRUCT(Blueprintable, BlueprintType)
struct FAvatarInfo2
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AvatarInfo")
		FName BaseKey;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AvatarInfo")
		FName TopKey;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AvatarInfo")
		FName BottomKey;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AvatarInfo")
		FName HeadKey;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AvatarInfo")
		FName HairKey;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AvatarInfo")
		FName ShoesKey;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AvatarInfo")
		FName BodyKey;
};

USTRUCT(Blueprintable, BlueprintType)
struct FTicketsInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Ticketinfo")
		int32 seq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Ticketinfo")
		FString iconImgUrl;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Ticketinfo")
		FString CodeName;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Ticketinfo")
		int32 TotalCount;
};

USTRUCT(Blueprintable, BlueprintType)
struct FTicketReward
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "RewardInfo")
	int32 TicketSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "RewardInfo")
	int32 Total;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "RewardInfo")
	FString TicketCode;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "RewardInfo")
	FString IconImgUrl;
};


USTRUCT(Blueprintable, BlueprintType)
struct FAttendContinuation
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Attendanceinfo")
		int32 CountAttendance;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Attendanceinfo")
		int32 RewardNumber;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Attendanceinfo")
		TArray<FTicketReward> TicketReward;
	
};

USTRUCT(BlueprintType, Blueprintable)
struct FTutorials
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	TMap<FString, int32> Tutorial;
};

USTRUCT(Blueprintable, BlueprintType)
struct FUserInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	FString Nickname;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	int32 Coin;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	int32 Crystal;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	FAvatarInfo2 AvatarInfo;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	TSet<FName> PurchasedCostumes;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	TSet<FName> PurchasedPacks;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	FName Flag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	FName Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	FName countrySP;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	FString UserID;

	// 출석체크 보상순서
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	int32 AttendanceSequence;

	// 출석체크 보상유무((bool)0 or 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	bool AttendanceFlag;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	int32 VoiceCoin;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	FDateTime ServerTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	FString provider;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	FDateTime AccountEndTime;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	TArray<FTicketsInfo> Tickets;
	
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	FAttendContinuation AttendContinuation;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "UserInfo")
	int32 userState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	int32 npcID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	FActionPoint ActionPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInfo")
	TMap<FString, int32> Tutorials;
};

UENUM(BlueprintType)
enum class EAppStore : uint8
{
	E_Google UMETA(DisplayName = "Google"),
	E_Apple UMETA(DisplayName = "Apple"),
	E_Huawei UMETA(DisplayName = "Huawei"),
};

UENUM(BlueprintType)
enum class ELoginWithParseType : uint8
{
	None UMETA(DisplayName = "None"),
	Guest UMETA(DisplayName = "Guest"),
	Huawei UMETA(DisplayName = "Huawei"),
	Apple UMETA(DisplayName = "Apple"),
};

UENUM(BlueprintType)
enum class EEmailType : uint8
{
	Generic											UMETA(DisplayName = "Generic"),
	Meta_Invite										UMETA(DisplayName = "Meta_Invite"),
	RankingAttach_BallTrapping_Week					UMETA(DisplayName = "RankingAttach_BallTrapping_Week"),
	RankingAttach_Dart_501_Week						UMETA(DisplayName = "RankingAttach_Dart_501_Week"),
	RankingAttach_BallTrapping_Month				UMETA(DisplayName = "RankingAttach_BallTrapping_Month"),
	RankingAttach_Dart_501_Month					UMETA(DisplayName = "RankingAttach_Dart_501_Month"),
	MarketAttachItem_Refund_By_Canceling			UMETA(DisplayName = "MarketAttachItem_Refund_By_Canceling"),
	MarketAttachItem_Success_Buying					UMETA(DisplayName = "MarketAttachItem_Success_Buying"),
	MarketAttachCoin_Refund_By_Canceling			UMETA(DisplayName = "MarketAttachCoin_Refund_By_Canceling"),
	MarketAttachCoin_Failed_Bidding					UMETA(DisplayName = "MarketAttachCoin_Failed_Bidding"),
	MarketAttachCoin_Failed_By_BuyingImmediately	UMETA(DisplayName = "MarketAttachCoin_Failed_By_BuyingImmediately"),
	MarketAttachCoin_Success_Selling				UMETA(DisplayName = "MarketAttachCoin_Success_Selling"),
	WithAttachment									UMETA(DisplayName = "WithAttachment"),
	RM_AccountConnectionReward						UMETA(DisplayName = "RM_AccountConnectionReward"),
	RM_Event_Reward									UMETA(DisplayName = "RM_Event_Reward"),
	RM_Event_AttendReward							UMETA(DisplayName = "RM_Event_AttendReward"),
	RM_Event_EndReward								UMETA(DisplayName = "RM_Event_EndReward"),
	Receive_Gift									UMETA(DisplayName = "Receive_Gift")
};

USTRUCT(Blueprintable, BlueprintType)
struct FConfigInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		FString ContentURL;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		EAppStore AppStore;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		FString Version;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		int32 Revision;
	/*UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		int32 Major;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		int32 Minor;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		int32 Patch;*/
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		FString UpdateURL;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		FString ManifestURL;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		FString CloudURL;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		TArray<FString> InstallDirs;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "ConfigInfo")
		FString PhotonVersion;
	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		FString PhotonProtocol;

	UPROPERTY(EditAnyWhere, BluePrintReadOnly, Category = "ConfigInfo")
		FDateTime WorkoutTime;
};

UENUM(BlueprintType)
enum class EItemType : uint8 // RewardType
{
	None,
	Goods UMETA(DisplayName = "Coin"),
	Costume UMETA(DisplayName = "Costume"),
	VoiceCoin UMETA(DisplayName = "VoiceCoin"),
	NormalTicket UMETA(DisplayName = "NormalTicket"),
	VipTicket UMETA(DisplayName = "VipTicket"),
};

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	None,
	Attendance,
};


UENUM(BlueprintType)
enum class EPartySorting : uint8
{
	Normal,
	Like,
};


USTRUCT(Blueprintable, BlueprintType)
struct FPartyRoom
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		int32 RoomID;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FName RowName;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FString Title;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FString UserName;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		int32 MasterUserSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		int32 MaxUser;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		int32 LikeCount;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		bool IsFixedRoom;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		bool IsShare;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FString PW;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		int32 CurrentUser;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FDateTime EndTime;
};

USTRUCT(Blueprintable, BlueprintType)
struct FAttachList
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AttachList")
		int attachmentSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AttachList")
		int itemID;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AttachList")
		bool isAlreadyGet;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "AttachList")
		EItemType itemType;
};

USTRUCT(Blueprintable, BlueprintType)
struct FEmailData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		int32 mailSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		FString sendName;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		FString mailTitle;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		FString sendDate;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		FString expiryDate;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		EEmailType mailType;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		FString roomTitle;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		int32 haveAttach;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "EmailData")
		TArray<FAttachList> attachList;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmailData")
		bool bAlreadyRead = false;
};


USTRUCT(Blueprintable, BlueprintType)
struct FSearchedUser
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		int32 UserSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FString Nickname;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FName Flag;
	/*UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "PartyRoom")
		FDateTime LastLoginTime;*/
};


UENUM(BlueprintType)
enum class EGameRankType : uint8
{
	RANKTYPE_All,
	RANKTYPE_Week,
	RANKTYPE_Month
};

UENUM(BlueprintType)
enum class EGameType : uint8
{
	None,
	BallTrapping,
	Dart_501,
	FreeKick,
};

USTRUCT(Blueprintable, BlueprintType)
struct FMiniGamePageInfo
{
	GENERATED_USTRUCT_BODY()
public:
	// user Page Info
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "MiniGamePageInfo")
		int32 currentPage;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "MiniGamePageInfo")
		int32 prevPage;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "MiniGamePageInfo")
		int32 nextPage;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "MiniGamePageInfo")
		int32 firstPage;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "MiniGamePageInfo")
		int32 lastPage;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "MiniGamePageInfo")
		int32 loadCount;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "MiniGamePageInfo")
		int32 totalPlayer;
};

USTRUCT(Blueprintable, BlueprintType)
struct FMiniGameUserInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameUserInfo")
		int64 userSeq;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameUserInfo")
		FString score;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameUserInfo")
		int32 rank;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameUserInfo")
		FString nickName;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameUserInfo")
		FString regDate;
};


USTRUCT(Blueprintable, BlueprintType)
struct FMiniGameInfo
{
	GENERATED_USTRUCT_BODY()
public:
	// Page Info
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameInfo")
		FMiniGamePageInfo pageInfo;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameInfo")
		FMiniGameUserInfo myInfo;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "MiniGameInfo")
		TArray<FMiniGameUserInfo> userInfoArr;
};

USTRUCT(Blueprintable, BlueprintType)
struct FRewardTable
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		EGameType gameSeq;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		EGameRankType rankType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		EItemType rewardType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		int rankGrade;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		int rewardID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		int rewardCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		int rewardReplacementPrice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RewardTable")
		bool activated;
};



UENUM(BlueprintType)
enum class EMarketSorting : uint8
{
	BidPrice,
	BidCount,
	WatchsCount,
	Reg
};

UENUM(BlueprintType)
enum class EAuctionCategory : uint8
{
	All,
	Shirts,
	Pants,
	Shoes,
	ETC
};


UENUM(BlueprintType)
enum class EMyMarketCategory : uint8
{
	Bidding,
	Sale,
	Watch
};


USTRUCT(Blueprintable, BlueprintType)
struct FAuctionItem
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		FString AuctionNumber;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		FName ItemID;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		int32 SellUserSeq;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		int32 BidUserSeq;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		FString Title;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		int32 BuyPrice;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		int32 CurrentBidPrice;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		int32 WatchCount;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		int32 BidCount;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Auction")
		FDateTime RegDate;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Auction")
		FDateTime EndDate;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Auction")
		int32 Status;
};




/* DEPRECATED RM_EVENT REGION */
#pragma region RM_EVENT
UENUM(BlueprintType)
enum class EEventType : uint8
{
	Normal,				// 일반성
	Random,				// 랟덤확률성
	Consume				// 구매성
};


USTRUCT(Blueprintable, BlueprintType)
struct FEventInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int eventSeq;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int priority;
	
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		FString eventCode;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		FString title;
	
	/* 날짜 제한이 있는 이벤트인지 확인 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		bool bUseLimitDate;
	/* StartDay, EndDay는 bUseLimitDate가 true일때만 노출됨. */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		FDateTime startDay;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		FDateTime endDay;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int limitDays;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
		bool bIsAttendable;
	/* 노출 순서 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int sortNum;

	/* [!]마크 노출 여부 (NEW) */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		bool bIsNew;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		EEventType eventType;

	/* 소모성이 있는 이벤트인지 확인(ex.유니폼 이벤트) */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		bool bIsConsumable;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int consumeCount;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int consumeMax;

	/* 1일 1회 참석 이벤트인지 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		bool limitPerOneDay;
	/* 모든 사람의 해당 이벤트 참여 횟수 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int totalAttendCount;
	/* 나의 해당 이벤트 참여 횟수 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int myTotalAttendCount;

	/* 이벤트 참여시 기본 보상 코인 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int baseCoin;
	/* 이벤트 종료시 기본 보상 코인 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int afterCoin;

	/* 단 한 번만 참여할 수 있는지 여부 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		bool bIsOnlyOne;

	/* 이벤트 참가비 */
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Event")
		int attendCoin;
};


USTRUCT(Blueprintable, BlueprintType)
struct FCardRankInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankInfo")
		int32 wincount;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankInfo")
		FString nickName;
};

USTRUCT(Blueprintable, BlueprintType)
struct FCardListElem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		int32 seq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		int32 id;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		FString cardName;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		int32 quantity;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		FString grade;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		int32 level;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		FString position;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		int32 power;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		FString club;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		FString contry;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		FString season;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		int32 userSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		FString uid;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardListElem")
		int32 iGrade;


	
};

USTRUCT(Blueprintable, BlueprintType)
struct FCardGameResult
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardGameResult")
		int32 coin;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardGameResult")
		int32 crystal;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardGameResult")
		int32 nowTrophy;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardGameResult")
		int32 preTrophy;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardGameResult")
		int32 trophyBoostPoint;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardGameResult")
		float trophyBoostPercent;
	


};

USTRUCT(Blueprintable, BlueprintType)
struct FGachaResult
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "GachaResult")
		int32 seq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "GachaResult")
		int32 cardDataSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "GachaResult")
		int32 id;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "GachaResult")
		int32 level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GachaResult")
		int32 number; // 광고 횟수 5회

};

USTRUCT(Blueprintable, BlueprintType)
struct FCardRankElem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankElem")
		int32 rank;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankElem")
		int32 trophy;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankElem")
		int32 clubSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankElem")
		FString countrySP;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankElem")
		FString nickName;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardRankElem")
		int32 userSeq;
};

USTRUCT(Blueprintable, BlueprintType)
struct FFriendFollowRankElem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "FriendFollowRankElem")
		FString userSeq;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "FriendFollowRankElem")
		FString nickname;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "FriendFollowRankElem")
		int32 club;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "FriendFollowRankElem")
		FString countrySP;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "FriendFollowRankElem")
		int32 followCnt;
};


USTRUCT(Blueprintable, BlueprintType)
struct FCardPackList
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardPackList")
		int32 infoSeq;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardPackList")
		FString gachaPackId;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardPackList")
		FString name;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardPackList")
		FString type;
	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "CardPackList")
		int32 coin;
};


USTRUCT(Blueprintable, BlueprintType)
struct FMyEventAttendanceInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "EventAttendance")
		int eventSeq;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "EventAttendance")
		FString eventCode;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "EventAttendance")
		int userSeq;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "EventAttendance")
		FDateTime regDate;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "EventAttendance")
		FString returnValue;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "EventAttendance")
		int attendTotalCount;

};

USTRUCT(Blueprintable, BlueprintType)
struct FEventWinnerUserInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinnerInfo")
	int eventSeq;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinnerInfo")
	int userSeq;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinnerInfo")
	FString nickName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinnerInfo")
	int rank;
};

USTRUCT(Blueprintable, BlueprintType)
struct FEventWinnerInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	int GiveCoin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	bool IsWinner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	int EventSeq;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	FString EventCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	int UserSeq;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	FDateTime RegDate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	int Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	int TotalAttendCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	FString Val;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	FString Val2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventWinner")
	TArray<FEventWinnerUserInfo> WinnersUserInfo;
};

USTRUCT(Blueprintable, BlueprintType)
struct FDailyQuestInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DailyQuestInfo")
	int32 dart=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DailyQuestInfo")
	int32 freekick=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DailyQuestInfo")
	int32 card=0;
};

USTRUCT(Blueprintable, BlueprintType)
struct FScreenDisplayInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreenDisplayInfo")
	int32 seq;
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreenDisplayInfo")
	FString title;
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreenDisplayInfo")
	FString url;
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreenDisplayInfo")
	FString area;
};

#pragma endregion

/* NEW RM_EVENT REGION */
#pragma region RM_EVENT
USTRUCT(Blueprintable, BlueprintType)
struct FEventInfoList
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventListInfo")
	FString EventCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventListInfo")
	int32 eventSeq;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventListInfo")
	bool bCanAttend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventListInfo")
	FString Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventListInfo")
	FString URL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EventListInfo")
	bool bIsNew;
};


#pragma endregion 

#pragma region RM_NOTICE
UENUM(Blueprintable, BlueprintType)
enum class ENoticeBoardType : uint8
{
	News = 0,
	Event,
};


USTRUCT(Blueprintable, BlueprintType)
struct FNoticeBoardInfo
{
	GENERATED_USTRUCT_BODY()
public:
	/* Board's Seqeunce */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeBoardInfo")
	int boardSeq;

	/* Board's Unique Code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeBoardInfo")
	FString boardCode;

	/* Board's View Country Code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeBoardInfo")
	FString countryCode;

	/* ListItem Title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeBoardInfo")
	FString title;

	/* List SubTitles */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeBoardInfo")
	FString subTitle;

	/* Register Date */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeBoardInfo")
	FDateTime regDate;

	/* WebView URL */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeBoardInfo")
	FString URL;
};
#pragma endregion 


#pragma region RM_Photo

USTRUCT(Blueprintable, BlueprintType)
struct FPhotoInfo
{
	GENERATED_USTRUCT_BODY()
public:
	/* 사진의 제목입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		int32 Seq;

	/* 사진을 등록한 유저의 닉네임입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		FString UserNickname;

	/* 사진이 저장된 CDN 주소입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		FString ImageUrl;

	/* 사진의 썸네일이 저장된 CDN 주소입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		FString ThumbnailUrl;

	/* 사진을 등록할 때 선택한 프레임의 번호입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		int32 FrameNumber;

	/* 사진을 좋아요 한 개수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		int32 CountLike;

	/* 자신이 사진을 좋아요 했는지 여부입니다. Bearer Token 인증에 실패했을 경우 무조건 false가 리턴됩니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		bool IsSelected;

	///* Register Date */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
	//	int32 ReportCount;

	/* 자신이 사진을 신고 했는지 여부입니다. Bearer Token 인증에 실패했을 경우 무조건 false가 리턴됩니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoInfo")
		bool IsReported;
};
#pragma endregion 

