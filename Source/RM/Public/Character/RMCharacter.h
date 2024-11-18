#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RMDefinedStruct.h"
#include "Character/RMPlayerAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RMCharacter.generated.h"

UCLASS()
class RM_API ARMCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARMCharacter();

public:
	UFUNCTION(BlueprintCallable)
	void RandScale();

	UFUNCTION(BlueprintCallable)
	void RandSkinColor();

	UFUNCTION(BlueprintCallable)
	void RandEyeColor();

	UFUNCTION(BlueprintCallable)
	void RandHandScale();

	UFUNCTION(BlueprintCallable)
	void RandTopSize();

	UFUNCTION(BlueprintCallable)
	void RandBottomSize();

	UFUNCTION(BlueprintCallable)
	void RandCheekSize();

	UFUNCTION(BlueprintCallable)
	void RandMorpherTarget(EMorphGroup MorphGroup);

	UFUNCTION(BlueprintCallable)
	void RandCostumePart(ECostumePart CostumePart);

	/* Get Surface Type */
	UFUNCTION()
	void UpdateFloorSurfaceType();

public:
	UFUNCTION(BlueprintCallable)
	void SetGender(EGenderRM Gender);
	
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
	void UpdateLOD();

	/* Surface Type for playalbeCharacter */
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> CurrentSurface;

	/* protected --> public */
	URMPlayerAnimInstance* GetAnimInst();
protected:

	FPlayerCharacter &GetMyPlayer();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);

protected:
	UFUNCTION(BlueprintCallable)
	void Load();

	UFUNCTION(BlueprintCallable)
	bool ChangeCostumePart(ECostumePart CostumePart, FName CostumeRowName);

	UFUNCTION(BlueprintCallable)
	void ChangeSkeletalMesh(FName Tag,USkeletalMesh* NewMesh,UMaterialInstance* NewMaterial);

// private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UMaterialInstanceDynamic*> Material;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(ExposeOnSpawn="true"))
	EGenderRM Gender;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	bool CustomLOD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	int32 CurrentCustomLOD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	TArray<float> CustomLODDistanceRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "CustomLOD"));
	FVector2D CustomLODLevelRange = FVector2D(0,2);

	/* TimerHandle For Update Current Floor SurfaceType */
	UPROPERTY()
	FTimerHandle UpdateSurfaceHandle;
};