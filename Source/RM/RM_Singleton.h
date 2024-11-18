// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "RM_UserDefinedStruct.h"
#include "RMDefinedEnum.h"
#include "RMDefinedStruct.h"
#include "RM_Singleton.generated.h"

UCLASS(Blueprintable, BlueprintType)
class RM_API URM_Singleton : public UObject
{
	GENERATED_BODY()

	URM_Singleton();
public:
   
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "CCS|Singleton")
	static URM_Singleton* GetSingleton(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	bool CheckAvailablePeriod();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FTimeInfo TimeInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable|Costume")
	UDataTable* CostumeDatatable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable|Costume")
	UDataTable* CharacterPreset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable|Costume")
	UDataTable* MetaPreset;
	
	UFUNCTION(BlueprintCallable)
	bool CheckChanged();

	UFUNCTION(BlueprintCallable)
	FPlayerCharacter& GetPlayerCharacter(int Index = 0);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void CreatePlayerSlot(const UObject* WorldContextObject,EGenderRM Gender,const FString &Name);
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerSlot(int Slot, FPlayerCharacter PlayerCharacter);

	static FPlayerCharacter GetDefaultCharacter();

	UPROPERTY(BlueprintReadOnly, Category = "Option|Graphic Quality")
	int32 SelectedSlot;

	UPROPERTY(BlueprintReadOnly, Category = "Option|Graphic Quality")
	TArray<FPlayerCharacter> PlayerSlot;

protected:
	TArray<FMorphValue> DefaultMorphValue;
	TMap<EGenderRM,TMap<ECostumePart,FName>> DefaultCostume;

private:
	static URM_Singleton* Instance;
};