// Fill out your copyright notice in the Description page of Project Settings.


#include "RM_Singleton.h"

#include "RMDataTable.h"

URM_Singleton* URM_Singleton::Instance = nullptr;


URM_Singleton::URM_Singleton()
{

	
	SelectedSlot = 0;

	TMap<ECostumePart, FName> Male;
	TMap<ECostumePart, FName> Female;
	TMap<ECostumePart, FName> Male2;
	TMap<ECostumePart, FName> Female2;
	TMap<ECostumePart, FName> Male3;
	TMap<ECostumePart, FName> Female3;
	TMap<ECostumePart, FName> Male4;
	TMap<ECostumePart, FName> Female4;
	

	DefaultMorphValue.Add(FMorphValue(TEXT("EyeScale+-"),EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("EyeWide+-"),EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("EyeLidShapeA+-"),EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("Termple"), EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("BrowUpdown+-"), EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("Cheek"), EMorphGroup::Face,0,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("Chin"), EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("ChinTip+-"), EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("NoseHigh+-"), EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("LipThic+-"), EMorphGroup::Face,-1, 1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("LipWide+-"), EMorphGroup::Face,-1, 1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("NoseShapeA+"), EMorphGroup::Face,-1,1,0));
	DefaultMorphValue.Add(FMorphValue(TEXT("NoseTip+-"), EMorphGroup::Face,-1,1,0));

	Male.Add(ECostumePart::Hair,TEXT("210001"));
	Male.Add(ECostumePart::Top,TEXT("410001"));
	Male.Add(ECostumePart::Bottom,TEXT("510001"));
	Male.Add(ECostumePart::Shoes,TEXT("610001"));
	Male.Add(ECostumePart::Head,TEXT("121001"));
	
	Male2.Add(ECostumePart::Hair,TEXT("210002"));
	Male2.Add(ECostumePart::Top,TEXT("410011"));
	Male2.Add(ECostumePart::Bottom,TEXT("510007"));
	Male2.Add(ECostumePart::Shoes,TEXT("610007"));
	Male2.Add(ECostumePart::Head,TEXT("121002"));
	
	Male3.Add(ECostumePart::Hair, TEXT("210003"));
	Male3.Add(ECostumePart::Top,TEXT("410021"));
	Male3.Add(ECostumePart::Bottom,TEXT("510013"));
	Male3.Add(ECostumePart::Shoes,TEXT("610013"));
	Male3.Add(ECostumePart::Head,TEXT("121011"));
	
	Male4.Add(ECostumePart::Hair, TEXT("210004"));
	Male4.Add(ECostumePart::Top,TEXT("410031"));
	Male4.Add(ECostumePart::Bottom,TEXT("510019"));
	Male4.Add(ECostumePart::Shoes,TEXT("610019"));
	Male4.Add(ECostumePart::Head,TEXT("121012"));
	
	Female.Add(ECostumePart::Hair,TEXT("210005"));
	Female.Add(ECostumePart::Top,TEXT("410041"));
	Female.Add(ECostumePart::Bottom,TEXT("510025"));
	Female.Add(ECostumePart::Shoes,TEXT("610025"));
	Female.Add(ECostumePart::Head,TEXT("122001"));
	
	Female2.Add(ECostumePart::Hair,TEXT("210006"));
	Female2.Add(ECostumePart::Top,TEXT("410043"));
	Female2.Add(ECostumePart::Bottom,TEXT("510031"));
	Female2.Add(ECostumePart::Shoes,TEXT("510031"));
	Female2.Add(ECostumePart::Head,TEXT("122002"));
	
	Female3.Add(ECostumePart::Hair,TEXT("210007"));
	Female3.Add(ECostumePart::Top,TEXT("410045"));
	Female3.Add(ECostumePart::Bottom,TEXT("510037"));
	Female3.Add(ECostumePart::Shoes,TEXT("510037"));
	Female3.Add(ECostumePart::Head,TEXT("122011"));
	//Translatetext
	Female4.Add(ECostumePart::Hair,TEXT("210008"));
	Female4.Add(ECostumePart::Top,TEXT("410047"));
	Female4.Add(ECostumePart::Bottom,TEXT("510043"));
	Female4.Add(ECostumePart::Shoes,TEXT("510043"));
	Female4.Add(ECostumePart::Head,TEXT("122012"));
	// Man Uniform   410001,411001,411002,411003
	// Woman Uniform 420001,420002,421001,421002
	
	//피부는 코드값이 없음
	// Female.Add(ECostumePart::FaceSkin,TEXT("women_fs01"));
	//Female.Add(ECostumePart::EyeBrow,TEXT("132001"));
	//Female.Add(ECostumePart::Lip,TEXT("142001"));

	DefaultCostume.Add(EGenderRM::Male, Male);
	DefaultCostume.Add(EGenderRM::Female,Female);
	DefaultCostume.Add(EGenderRM::Male2,Male2);
	DefaultCostume.Add(EGenderRM::Female2,Female2);
	DefaultCostume.Add(EGenderRM::Male3, Male3);
	DefaultCostume.Add(EGenderRM::Female3, Female3);
	DefaultCostume.Add(EGenderRM::Male4, Male4);
	DefaultCostume.Add(EGenderRM::Female4, Female4);

	PlayerSlot.Empty();
	CreatePlayerSlot(GetWorld(), EGenderRM::Male, FString::Printf(TEXT("0")));
	CreatePlayerSlot(GetWorld(), EGenderRM::Male, FString::Printf(TEXT("1")));
}

URM_Singleton* URM_Singleton::GetSingleton(const UObject* WorldContextObject)
{
	URM_Singleton* Singleton = Cast<URM_Singleton>(GEngine->GameSingleton);
	if (!Singleton) return nullptr;

	Instance = Singleton;
	return Singleton;
}

bool URM_Singleton::CheckAvailablePeriod()
{
	return FDateTime::UtcNow() < GetSingleton(GetWorld())->TimeInfo.AccountEndTime;
}

FPlayerCharacter& URM_Singleton::GetPlayerCharacter(int Index)
{
	UDataTable* DT;
	FSoftObjectPath MetapresetPath(TEXT("DataTable'/Game/PakDataTable/DT_MetaPreset.DT_MetaPreset'"));
	DT = Cast<UDataTable>(MetapresetPath.ResolveObject());
	if (DT ==nullptr)
	{
		DT = CastChecked<UDataTable>(MetapresetPath.TryLoad());
	}
	if (DT!=nullptr)
	{
		MetaPreset = DT;	
	}

	UDataTable* RMDT;
	FSoftObjectPath CostumePath(TEXT("DataTable'/Game/PakDataTable/NewCostumeTable.NewCostumeTable'"));
	RMDT = Cast<UDataTable>(CostumePath.ResolveObject());
	if (RMDT == nullptr)
	{
		RMDT = CastChecked<UDataTable>(CostumePath.TryLoad());
	}
	if (RMDT!=nullptr)
	{
		CostumeDatatable = RMDT;	
	}
	
	UDataTable* CharacterDT;
	FSoftObjectPath CharacterDTPath(TEXT("DataTable'/Game/PakDataTable/01_CharacterPresetTable.01_CharacterPresetTable'"));
	CharacterDT = Cast<UDataTable>(CharacterDTPath.ResolveObject());
	if (CharacterDT == nullptr)
	{
		CharacterDT = CastChecked<UDataTable>(CharacterDTPath.TryLoad());
	}
	if (CharacterDT!=nullptr)
	{
		CharacterPreset = CharacterDT;	
	}

	if (PlayerSlot.Num() == 0)
	{
		CreatePlayerSlot(GetWorld(), EGenderRM::Male, FString::Printf(TEXT("Name=%d"), FMath::Rand()));
	}
	return PlayerSlot[Index];
}

FPlayerCharacter URM_Singleton::GetDefaultCharacter()
{
	
	
	FPlayerCharacter Player;

	if (Instance)
	{	
		/*Player.Name = Name;
		Player.Gender = Gender;*/
		// Player.BirthDay = UTime::ToString(WorldContextObject, false);
		Player.MorphTargetValue = Instance->DefaultMorphValue;
		Player.Costume = Instance->DefaultCostume[EGenderRM::Male];
	}

	Player.TopSize = 0;
	Player.BottomSize = 0;
	Player.CheekSize = 0;
	Player.Scale = 1.0f;
	Player.HeadScale = 1.0f;
	Player.HandScale = 1.0f;
	Player.SkinColor = FLinearColor::White;
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::EyeColor, 0);
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::IrisBrightness, 1.5);
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::PupilScale, 0.8f);
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::ScleraBrightness, 1.0f);
	
	return Player;
}

void URM_Singleton::CreatePlayerSlot(const UObject* WorldContextObject,EGenderRM Gender, const FString &Name)
{
	

	
	FPlayerCharacter Player;
	Player.Name = Name;
	Player.Gender = Gender;
	// Player.BirthDay = UTime::ToString(WorldContextObject, false);
	Player.MorphTargetValue = DefaultMorphValue;
	Player.TopSize = 0;
	Player.BottomSize = 0;
	Player.CheekSize = 0;
	Player.Scale = 1.0f;
	Player.HeadScale = 1.0f;
	Player.HandScale = 1.0f;
	Player.SkinColor = FLinearColor::White;
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::EyeColor,0);
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::IrisBrightness,1.5);
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::PupilScale,0.8f);
	Player.EyeMaterialScalarParameter.Add(EEyeMaterialScalarParameter::ScleraBrightness,1.0f);
	Player.Costume = DefaultCostume[Gender];
	
	// if (PlayerSlot.Num() == 0)
		PlayerSlot.Add(Player);
	// else
	// 	PlayerSlot[0] = Player;
	// SelectedSlot = 0;//PlayerSlot.Num() - 1;
}

void URM_Singleton::SetPlayerSlot(int Slot, FPlayerCharacter PlayerCharacter)
{
	if (PlayerSlot.Num() > Slot)
		PlayerSlot[Slot] = PlayerCharacter;
}

bool URM_Singleton::CheckChanged()
{
	bool IsChanged = false;

	FPlayerCharacter CurrentData = GetPlayerCharacter(0);
	FPlayerCharacter SavedData = GetPlayerCharacter(1);

	if (CurrentData.Costume[ECostumePart::Head] != SavedData.Costume[ECostumePart::Head])
		return	IsChanged = true;
	if(CurrentData.Costume[ECostumePart::Top] != SavedData.Costume[ECostumePart::Top])
		return	IsChanged = true;
	
	if (CurrentData.Gender != SavedData.Gender
	|| CurrentData.TopSize != SavedData.TopSize
	|| CurrentData.BottomSize != SavedData.BottomSize
	|| CurrentData.CheekSize != SavedData.CheekSize
	|| CurrentData.Scale != SavedData.Scale
	|| CurrentData.HeadScale != SavedData.HeadScale
	|| CurrentData.HandScale != SavedData.HandScale
	// || CurrentData.SkinColor != SavedData.SkinColor)
	|| !CurrentData.SkinColor.Equals(SavedData.SkinColor))
	{
		return IsChanged = true;
	}

	for (int i = 0; i < CurrentData.MorphTargetValue.Num(); i++)
	{
		if (CurrentData.MorphTargetValue[i].Value != SavedData.MorphTargetValue[i].Value)
			return IsChanged = true;
	}

	for (int i = 0; i < CurrentData.EyeMaterialScalarParameter.Num(); i++)
	{
		if (CurrentData.EyeMaterialScalarParameter[(EEyeMaterialScalarParameter)i] != SavedData.EyeMaterialScalarParameter[(EEyeMaterialScalarParameter)i])
			return IsChanged = true;
	}
	
	return IsChanged;
}