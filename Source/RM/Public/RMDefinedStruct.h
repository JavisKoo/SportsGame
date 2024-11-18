#pragma once

#include <cwctype>
#include <cctype>
#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "RMDefinedEnum.h"
#include "ThirdParty/Utility.h"
#include "RMDefinedStruct.generated.h"

UCLASS()
class RM_API URMDefinedStuct : public UUserDefinedStruct
{
	GENERATED_BODY()


};

USTRUCT(Blueprintable)
struct FMorphValue
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMorphGroup Group;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	FMorphValue() {
		Name = FName(TEXT(""));
		Group = EMorphGroup::All;
		Min = -1.0f;
		Max =  1.0f;
		Value = 0.0f;
	}
	
	FMorphValue(FName InName,EMorphGroup InGroup, float InMin,	float InMax,float InValue) {
		Name = InName;
		Group = InGroup;
		Min = InMin;
		Max = InMax;
		Value = InValue;
	}

};

USTRUCT(Blueprintable)
struct FPlayerCharacter
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGenderRM Gender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BirthDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HandScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TopSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BottomSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CheekSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FacialBoneScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EEyeMaterialScalarParameter, float> EyeMaterialScalarParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor SkinColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMorphValue> MorphTargetValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ECostumePart, FName> Costume;
	
	FString ToJson();
};