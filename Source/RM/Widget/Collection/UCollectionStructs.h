#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Misc/DateTime.h"
#include "UCollectionStructs.generated.h"

USTRUCT(BlueprintType)
struct FCCollectionUIManagerSetting : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxPieces = 10;
};

USTRUCT(BlueprintType)
struct FCCollectionPlayerPiece : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Piece;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCollected;
};

USTRUCT(BlueprintType)
struct FCCollectionPlayerInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString FirstName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Country;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDateTime Birth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Hight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 UniformNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture* PlayerCardImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture* CollectionImage;
};