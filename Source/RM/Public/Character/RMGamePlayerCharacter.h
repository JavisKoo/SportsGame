#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RMDefinedStruct.h"
#include "Character/RMPlayerAnimInstance.h"
#include "RMDataTable.h"
#include "RM/RM_Singleton.h"
#include "Kismet/GameplayStatics.h"
#include "RMGamePlayerCharacter.generated.h"

UCLASS()
class RM_API ARMGamePlayerCharacter : public ACharacter 
{
	GENERATED_BODY()
	
public:
	ARMGamePlayerCharacter();

	UFUNCTION(BlueprintCallable)
	void SetHandScale(float Scale);
	
	UFUNCTION(BlueprintCallable)
	void SetTopSize(float Size);

	UFUNCTION(BlueprintCallable)
	void SetBottomSize(float Size);

	UFUNCTION(BlueprintCallable)
	void SetHeadScale(float Scale);

	UFUNCTION(BlueprintCallable)
	void SetScale(float Scale);

	UFUNCTION(BlueprintCallable)
	void SetCostumePart(ECostumePart Part, FName RowName);

	UFUNCTION(BlueprintCallable)
	void SetFacialBone(const FVector& Value);


	UFUNCTION(BlueprintCallable)
	void SetEyeScalaParameter(EEyeMaterialScalarParameter Enum, float Value);

	UFUNCTION(BlueprintCallable)
	void SetSkinColor(const FLinearColor &SkinColor);
	
	UFUNCTION(BlueprintCallable)
	void SetCheekSize(float Size);
	
	UFUNCTION(BlueprintCallable)
	void SetMorpherTargetValue(EMorphTarget Target,float Value);

	UFUNCTION(BlueprintCallable)
	void Load();

	UFUNCTION(BlueprintCallable)
	bool ChangeCostumePart(ECostumePart CostumePart, FName CostumeRowName);

	UFUNCTION(BlueprintCallable)
	void ChangeSkeletalMesh(FName Tag, USkeletalMesh* NewMesh, UMaterialInstance* NewMaterial);

	UFUNCTION(BlueprintCallable)
	void UpdateCustomLOD();

	/* protected --> public */
	URMPlayerAnimInstance* GetAnimInst();
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FPlayerCharacter PlayerCharacter;
	
	FPlayerCharacter &GetMyPlayer();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);
	
	// private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UMaterialInstanceDynamic*> Material;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	bool CustomLOD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	int32 CurrentCustomLOD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	TArray<float> CustomLODDistanceRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	FVector2D CustomLODLevelRange = FVector2D(0, 900);
};
