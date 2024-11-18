#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Engine/DataTable.h"
#include "RMDefinedEnum.h"
#include "../RMStruct.h"
#include "RMDataTable.generated.h"
/**
 * 
 */
UCLASS()
class RM_API URMDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
//ECostumePart
USTRUCT(Blueprintable)
struct FCostumeTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	bool Exclude;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	UTexture2D* SkinType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	UPaperSprite* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	EGenderRM Gender;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	EPBR Priority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	ECostumePart Part;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	bool IsCustomHead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	int32 MeshLOD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	int32 TextureLOD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	TSoftObjectPtr<UMaterialInstance> Material;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	TSoftObjectPtr<UTexture2D> Tex1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	TSoftObjectPtr<UTexture2D> Tex2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "코스튬")
	TSoftObjectPtr<UTexture2D> Tex3;
};



USTRUCT(Blueprintable)
struct FMapDownloadTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Download")
	ENewLevelType LevelType;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Download")
	int32 Version;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Download")
	FString URL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Download")
	FString InstallDir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Download")
	FName Dependency;
};


