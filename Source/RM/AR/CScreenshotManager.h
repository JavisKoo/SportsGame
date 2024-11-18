#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CScreenshotManager.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FScreenshotData
{
	GENERATED_BODY()

public:
	FScreenshotData()
		: LastIndex(INDEX_NONE)
	{}

	FScreenshotData(const int32 InLastIndex)
		: LastIndex(InLastIndex)
	{}

	UPROPERTY(BlueprintReadWrite)
		int32 LastIndex;
};

UCLASS()
class RM_API ACScreenshotManager : public AActor
{
	GENERATED_BODY()

public:
	ACScreenshotManager();

public:
	UFUNCTION(BlueprintCallable)
		FString Screenshot();

	UFUNCTION(BlueprintCallable)
		class UTexture2D* GetTextureFromPNG(const FString& FileName);
	UFUNCTION(BlueprintCallable)
		class UTexture2D* GetTextureFromPNGAndroid(const FString& Path, const FString& FileName);

	UFUNCTION(BlueprintCallable)
		void GetScreenshotNames(TArray<FString>& OutSceenshotNames);

	UFUNCTION(BlueprintCallable)
		void GetScreenshotNamesAndroid(const FString& Path, const FString& Name, TArray<FString>& OutSceenshotNames);

protected:
	virtual void BeginPlay() override;

private:
	void SaveScreenshotData(const int32 LastIndex);
	int32 GetScreenshotLastIndex();

private:
	static const int32 MaxScreenshotCount;
	FString file_path;
};
