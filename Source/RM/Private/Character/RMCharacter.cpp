#include "Character/RMCharacter.h"
#include "Character/RMGamePlayerCharacter.h"
#include "Character/RMPlayerAnimInstance.h"
#include "RMDataTable.h"
#include "RM/RM_Singleton.h"
#include "ThirdParty/Utility.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "RM/RMProtocol.h"


ARMCharacter::ARMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName(TEXT("RMCharacter")));
}

FPlayerCharacter & ARMCharacter::GetMyPlayer()
{
	return URM_Singleton::GetSingleton(GetWorld())->GetPlayerCharacter();
}

URMPlayerAnimInstance * ARMCharacter::GetAnimInst()
{
	URMPlayerAnimInstance *AnimInst = Cast<URMPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	return AnimInst;
}

void ARMCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UMaterialInstanceDynamic* BodySkin = GetMesh()->CreateDynamicMaterialInstance(0);
	if (BodySkin) {
		Material.Add(BodySkin);
	}
	TArray<UActorComponent*> Skel;//= GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	this->GetComponents(USkeletalMeshComponent::StaticClass(), Skel);
	for (auto It : Skel) {
		USkeletalMeshComponent* S = Cast<USkeletalMeshComponent>(It);
		if (S) {
			if (S->ComponentHasTag(FName(TEXT("Head")))) {
				UMaterialInstanceDynamic* HeadSkin = S->CreateDynamicMaterialInstance(0);
				if (HeadSkin) {
					Material.Add(HeadSkin);
				}

				UMaterialInstanceDynamic* Eye = S->CreateDynamicMaterialInstance(1);
				if (Eye) {
					Material.Add(Eye);
				}
			}
		}
	}
}

void ARMCharacter::BeginPlay()
{
	Super::BeginPlay();
	Load();
	const float TimerHandleDuration = 0.5f;
	GetWorld()->GetTimerManager().SetTimer(UpdateSurfaceHandle, FTimerDelegate::CreateUObject(this, &ARMCharacter::UpdateFloorSurfaceType), TimerHandleDuration, true);
}

void ARMCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UpdateLOD();
}


void ARMCharacter::Load()
{
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst == nullptr) {
		return;
	}
	
	FPlayerCharacter& Me = GetMyPlayer();

	//���۰� ����
	for (auto& It : Me.MorphTargetValue) {
		AnimInst->SetMorphTarget(It.Name, It.Value);
	}


	for (auto It : Me.Costume) {
		if (It.Value != TEXT("")) {
			ChangeCostumePart(It.Key, It.Value);
		}
	}



	//�� ������
	AnimInst->Scale = Me.Scale;

	//�� ������
	AnimInst->HandScale = Me.HandScale;


	AnimInst->FacialBoneScale = Me.FacialBoneScale;


	//��� �����Ͽ� ���� Ư�� ������ ������ �޴´�.
	AnimInst->HeadScale = Me.HeadScale;
	
	/*
	FVector2D Input(1.06f, 0.90302f);
	FVector2D Output1(1.0f, 0.0f);
	FVector2D Output2(0.0f, 1.0f);
	float V1 = FMath::GetMappedRangeValueClamped(Input, Output1, Me.HeadScale);
	float V2 = FMath::GetMappedRangeValueClamped(Input, Output2, Me.HeadScale);
	AnimInst->SetMorphTarget("HeadScaleUp", V1);
	AnimInst->SetMorphTarget("HeadScaleDown", V2);
	*/


	/*
	if (Me.HeadScale > 1.0f) {
		FVector2D Input(0.95f, 1.05);
		FVector2D Output(0.0f, 1.0f);
		float Value = FMath::GetMappedRangeValueClamped(Input,Output,Me.HeadScale);
		AnimInst->SetMorphTarget("HeadScaleUp", Value);
		AnimInst->SetMorphTarget("HeadScaleDown",0);
	}
	else {
		FVector2D Input(1.05,0.95f);
		FVector2D Output(0.0f, 1.0f);
		float Value = FMath::GetMappedRangeValueClamped(Input,Output,Me.HeadScale);
		AnimInst->SetMorphTarget("HeadScaleUp",0);
		AnimInst->SetMorphTarget("HeadScaleDown", Value);
	}
	*/

	//��ü ������
	if (Me.TopSize > 0) {
		AnimInst->SetMorphTarget("TopSlim",0.0f);
		AnimInst->SetMorphTarget("TopBig",Me.TopSize);
	}
	else {
		AnimInst->SetMorphTarget("TopSlim",FMath::Abs(Me.TopSize));
		AnimInst->SetMorphTarget("TopBig",0);
	}


	if (Me.BottomSize > 0.0f) {
		AnimInst->SetMorphTarget("BottomSlim", 0.0f);
		AnimInst->SetMorphTarget("BottomBig", Me.BottomSize);
	}
	else {
		AnimInst->SetMorphTarget("BottomSlim", FMath::Abs(Me.BottomSize));
		AnimInst->SetMorphTarget("BottomBig",0);
	}
	
	if (Me.CheekSize < 0) {
		AnimInst->SetMorphTarget("CheekBlow", 0);
		AnimInst->SetMorphTarget("CheekSuck", FMath::Abs(Me.CheekSize));
	}
	else {
		AnimInst->SetMorphTarget("CheekBlow", Me.CheekSize);
		AnimInst->SetMorphTarget("CheekSuck", 0);
	}

	if (Material.Num()) {
		//FColor
		UE_LOG(LogTemp, Log, TEXT("Load Set Color"));
		// for (auto It : Material) {
		// 	if (It) {
		// 		It->SetVectorParameterValue(FName(TEXT("SkinColor")), Me.SkinColor);
		// 	}
		// }
		// for (auto It : Me.EyeMaterialScalarParameter)
		// {
		// 	switch (It.Key) {
		// 	case EEyeMaterialScalarParameter::EyeColor:
		// 		Material[2]->SetScalarParameterValue(FName(TEXT("EyeColor")), It.Value);
		// 		break;
		// 	case EEyeMaterialScalarParameter::IrisBrightness:
		// 		Material[2]->SetScalarParameterValue(FName(TEXT("IrisBRightness")), It.Value);
		// 		break;
		// 	case EEyeMaterialScalarParameter::PupilScale:
		// 		Material[2]->SetScalarParameterValue(FName(TEXT("PupilScale")), It.Value);
		// 		break;
		// 	case EEyeMaterialScalarParameter::ScleraBrightness:
		// 		Material[2]->SetScalarParameterValue(FName(TEXT("ScleraBrightness")), It.Value);
		// 		break;
		//
		// 	}
		// }
	}
}

void ARMCharacter::RandMorpherTarget(EMorphGroup MorphGroup)
{
	FPlayerCharacter& Me = GetMyPlayer();
	for (auto& It : Me.MorphTargetValue) {
		//if (MorphGroup == EMorphGroup::All || It.Group == MorphGroup) {
		if (It.Group == MorphGroup) {
			It.Value = FMath::RandRange(It.Min, It.Max);
		}
	}
}

void ARMCharacter::RandScale()
{
	float Scale = FMath::RandRange(0.95f, 1.05f);
	SetScale(Scale);
}

void ARMCharacter::RandCostumePart(ECostumePart CostumePart)
{
	FPlayerCharacter& Me = GetMyPlayer();
	
	TArray<FName> Rows;
	UDataTable* DataTable = URM_Singleton::GetSingleton(GetWorld())->CostumeDatatable;
	if (DataTable && DataTable->GetRowMap().Num() > 0) {
		FString ContextString;
		for (FName RowName : DataTable->GetRowNames()) {
			FCostumeTable* Row = DataTable->FindRow<FCostumeTable>(RowName, ContextString);
			if (Row) {
				
				if (Row->Part == CostumePart && Me.Gender == Row->Gender) {
					Rows.Add(RowName);
				}
			}
		}
		if (Rows.Num() > 0) {
			FName RowName = Rows[FMath::RandRange(0, Rows.Num() - 1)];
			if (ChangeCostumePart(CostumePart, RowName)) {
				if (Me.Costume.Num() > 0) {
					Me.Costume[CostumePart] = RowName;
				}
			}
		}
	}
}

void ARMCharacter::UpdateFloorSurfaceType()
{
	FVector start = GetActorLocation();
	FVector end = start + (GetActorUpVector() * -2.0f * GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FHitResult outHit;
	TArray<AActor*> IgnoreActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARMGamePlayerCharacter::StaticClass(), IgnoreActors);

	/* if Use Simple Collision in mesh, Check Mesh's PhysicsMat */
	bool bIsHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end,
		ETraceTypeQuery::TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, outHit, true);
	if (bIsHit)
	{
		/* FOR DEBUG */
		UEngine* engine = UGameplayStatics::GetGameInstance(GetWorld())->GetEngine();
		if (engine && outHit.GetActor() != nullptr)
		{
			FString EnumToString;
			const UEnum* PhysicsMatEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhysicalSurface"), true);
			if (PhysicsMatEnum)
				EnumToString = PhysicsMatEnum->GetNameStringByValue(outHit.PhysMaterial != nullptr 
					? outHit.PhysMaterial->SurfaceType.GetValue()
					: EPhysicalSurface::SurfaceType_Default);

			//FString msg = TEXT(":::::::DEBUG SIMPLE LINETRACE SURFACE::::::: \n") + outHit.GetActor()->GetName() + EnumToString;
			//engine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, msg);
		}

		if (outHit.GetActor() != nullptr)
		{
			CurrentSurface = outHit.PhysMaterial.Get()->SurfaceType;
			
			if (CurrentSurface != EPhysicalSurface::SurfaceType_Default)
				return;
		}
	}
	
	/* if Use Complex Collision in mesh, Check Mesh's Material or Material Instance's PhysicsMat. */
	bIsHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end,
		ETraceTypeQuery::TraceTypeQuery1, true, IgnoreActors, EDrawDebugTrace::None, outHit, true);
	
	if (bIsHit)
	{
		/* FOR DEBUG */
		UEngine* engine = UGameplayStatics::GetGameInstance(GetWorld())->GetEngine();
		if (engine && outHit.GetActor() != nullptr)
		{
			FString EnumToString;
			const UEnum* PhysicsMatEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhysicalSurface"), true);
			if (PhysicsMatEnum)
				EnumToString = PhysicsMatEnum->GetNameStringByValue(outHit.PhysMaterial != nullptr
					? outHit.PhysMaterial->SurfaceType.GetValue()
					: EPhysicalSurface::SurfaceType_Default);

			//FString msg = TEXT(":::::::DEBUG COMPLEX LINETRACE SURFACE::::::: \n") + outHit.GetActor()->GetName() + EnumToString;
			//engine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, msg);
		}

		if (outHit.GetActor() != nullptr)
		{
			CurrentSurface = outHit.PhysMaterial.Get()->SurfaceType;

			if (CurrentSurface != EPhysicalSurface::SurfaceType_Default)
				return;
		}
	}

	/* by default, CurrentSurfaceType = SurfaceType_Defualt. */
	CurrentSurface = TEnumAsByte<EPhysicalSurface>(EPhysicalSurface::SurfaceType_Default);
}

void ARMCharacter::RandSkinColor()
{
	SetSkinColor(FColor::MakeRandomColor());
}

void ARMCharacter::RandEyeColor()
{
	//SetEyeColor(FMath::RandRange(0,255));
}

void ARMCharacter::RandHandScale()
{
	float Scale = FMath::RandRange(0.9f, 1.1f);
	SetHandScale(Scale);
}

void ARMCharacter::RandTopSize()
{
	float Size = FMath::RandRange(-1.0f, 1.0f);
	SetTopSize(Size);
}

void ARMCharacter::RandBottomSize()
{
	float Size = FMath::RandRange(-1.0f, 1.0f);
	SetBottomSize(Size);
}

void ARMCharacter::RandCheekSize()
{
	float Size = FMath::RandRange(-1.0f, 1.0f);
	SetCheekSize(Size);
}

void ARMCharacter::SetHandScale(float Scale)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.HandScale = Scale;
		AnimInst->HandScale = Scale;
	}
}

void ARMCharacter::SetTopSize(float Size)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.TopSize = Size;
		//��ü ������
		if (Me.TopSize > 0) {
			AnimInst->SetMorphTarget("TopSlim", 0.0f);
			AnimInst->SetMorphTarget("TopBig", Me.TopSize);
		}
		else {
			AnimInst->SetMorphTarget("TopSlim", FMath::Abs(Me.TopSize));
			AnimInst->SetMorphTarget("TopBig", 0);
		}
	}
}

void ARMCharacter::SetBottomSize(float Size)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.BottomSize = Size;
		if (Me.BottomSize > 0.0f) {
			AnimInst->SetMorphTarget("BottomSlim", 0.0f);
			AnimInst->SetMorphTarget("BottomBig", Me.BottomSize);
		}
		else {
			AnimInst->SetMorphTarget("BottomSlim", FMath::Abs(Me.BottomSize));
			AnimInst->SetMorphTarget("BottomBig", 0);
		}
	}
}

void ARMCharacter::SetHeadScale(float Scale)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		//��� �����Ͽ� ���� Ư�� ������ ������ �޴´�.
		Me.HeadScale = Scale;
		AnimInst->HeadScale = Me.HeadScale;
		
		/*
		FVector2D Input(1.06f, 0.90302f);
		FVector2D Output1(1.0f,0.0f);
		FVector2D Output2(0.0f,1.0f);
		float V1 = FMath::GetMappedRangeValueClamped(Input, Output1, Me.HeadScale);
		float V2 = FMath::GetMappedRangeValueClamped(Input, Output2, Me.HeadScale);
		AnimInst->SetMorphTarget("HeadScaleUp", V1);
		AnimInst->SetMorphTarget("HeadScaleDown", V2);
		*/






/*
		if (Me.HeadScale > 1.0f) {
			FVector2D Input(0.90302f, 1.06f);
			FVector2D Output(0.0f, 1.0f);
			float Value = FMath::GetMappedRangeValueClamped(Input,Output,Me.HeadScale);
			AnimInst->SetMorphTarget("HeadScaleUp",Value);
			AnimInst->SetMorphTarget("HeadScaleDown",0);
		}
		else {
			FVector2D Input(1.06f, 0.90302f);
			FVector2D Output(0.0f, 1.0f);
			
			
			float Value = FMath::GetMappedRangeValueClamped(Input,Output,Me.HeadScale);
			AnimInst->SetMorphTarget("HeadScaleUp",0);
			AnimInst->SetMorphTarget("HeadScaleDown",Value);
		}
		*/
	}
}

void ARMCharacter::SetGender(EGenderRM ThisGender)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInstance = GetAnimInst();

	if (AnimInstance)
	{
		Me.Gender = ThisGender;
		Gender = ThisGender;
	}
}

void ARMCharacter::SetScale(float Scale)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInstance = GetAnimInst();
	if (AnimInstance) {
		Me.Scale = Scale;
		AnimInstance->Scale = Scale;
	}
}

void ARMCharacter::SetCostumePart(ECostumePart Part, FName RowName)
{
	FPlayerCharacter& Me = GetMyPlayer();
	if (ChangeCostumePart(Part, RowName))
	{
		Me.Costume[Part] = RowName;

		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString(RowName.ToString()));
	}
} 

void ARMCharacter::SetFacialBone(const FVector& Value)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInstance = GetAnimInst();
	if (AnimInstance) {
		Me.FacialBoneScale = Value;
		AnimInstance->FacialBoneScale = Value;
	}

}

void ARMCharacter::SetEyeScalaParameter(EEyeMaterialScalarParameter Enum,float Value)
{
	/*
	FPlayerCharacter& Me = GetMyPlayer();
	switch (Enum) {
	case EEyeMaterialScalarParameter::EyeColor:
		Material[2]->SetScalarParameterValue(FName(TEXT("EyeColor")), Value);
		break;
	case EEyeMaterialScalarParameter::IrisBrightness:
		Material[2]->SetScalarParameterValue(FName(TEXT("IrisBRightness")), Value);
		break;
	case EEyeMaterialScalarParameter::PupilScale:
		Material[2]->SetScalarParameterValue(FName(TEXT("PupilScale")), Value);
		break;
	case EEyeMaterialScalarParameter::ScleraBrightness:
		Material[2]->SetScalarParameterValue(FName(TEXT("ScleraBrightness")), Value);
		break;
	}
	Me.EyeMaterialScalarParameter[Enum] = Value;
	*/
}


void ARMCharacter::SetSkinColor(const FLinearColor &SkinColor)
{
	FPlayerCharacter& Me = GetMyPlayer();
	if (Material.Num()) {
		//FColor
		Me.SkinColor = SkinColor;
		for (auto It : Material) {
			if (It) {
				It->SetVectorParameterValue(FName(TEXT("SkinColor")), Me.SkinColor);
			}
		}
	}
}

void ARMCharacter::SetCheekSize(float Size)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.CheekSize = Size;
		if (Me.CheekSize < 0) {
			AnimInst->SetMorphTarget("CheekBlow", 0);
			AnimInst->SetMorphTarget("CheekSuck", FMath::Abs(Me.CheekSize));
		}
		else {
			AnimInst->SetMorphTarget("CheekBlow", Me.CheekSize);
			AnimInst->SetMorphTarget("CheekSuck", 0);
		}
	}
}

bool ARMCharacter::ChangeCostumePart(ECostumePart CostumePart, FName CostumeRowName)
{
	UDataTable* DataTable = URM_Singleton::GetSingleton(GetWorld())->CostumeDatatable;
	if (DataTable && DataTable->GetRowMap().Num() > 0) {
		FString ContextString;
		FCostumeTable* Row = DataTable->FindRow<FCostumeTable>(CostumeRowName, ContextString);
		if (Row) {
			if (Row->Part == CostumePart) {
				switch (CostumePart) {
				case ECostumePart::Hair:
				//case ECostumePart::Head:
				case ECostumePart::Top:
				case ECostumePart::Bottom:
				case ECostumePart::Shoes: {
						FString S = EnumToString(TEXT("ECostumePart"),(uint8)CostumePart);
						
						if (Row->SkeletalMesh.IsPending()) Row->SkeletalMesh.LoadSynchronous();
						if (Row->Material.IsPending()) Row->Material.LoadSynchronous();
						ChangeSkeletalMesh(*S, Row->SkeletalMesh.Get(), Row->Material.Get());
						
						if (CostumePart == ECostumePart::Head) {
							if (Row->IsCustomHead)
							{
								if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
								if (Row->Tex2.IsPending()) Row->Tex2.LoadSynchronous();
								if (Row->Tex2.IsPending()) Row->Tex3.LoadSynchronous();
								
								Material[1]->SetTextureParameterValue("Defuse", Row->Tex1.Get());
								Material[1]->SetTextureParameterValue("Normal", Row->Tex2.Get());
								Material[1]->SetTextureParameterValue("split_AM_R_M",Row->Tex3.Get());

								FPlayerCharacter& Me = GetMyPlayer();
								SetSkinColor(Me.SkinColor);
							}
							else
							{
								UMaterialInstance* PresetHeadMaterial = Row->Material.Get();
								if (PresetHeadMaterial)
								{
									FLinearColor OutValue;
									PresetHeadMaterial->GetVectorParameterValue(FHashedMaterialParameterInfo("SkinColor"), OutValue);
									// Material[0]->SetVectorParameterValue("SkinColor", OutValue);
									// SetSkinColor(OutValue);
									for (auto It : Material) {
										if (It) {
											It->SetVectorParameterValue(FName(TEXT("SkinColor")), OutValue);
										}
									}
								}
							}
						}
					}
					return true;
					/*
				case ECostumePart::FaceSkin:
					if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
					Material[1]->SetTextureParameterValue("Defuse", Row->Tex1.Get());
					return true;

				case ECostumePart::EyeBrow:
					if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
					Material[1]->SetTextureParameterValue("EyeBrowType", Row->Tex1.Get());
					return true;

				case ECostumePart::Lip:
					if (Row->Tex1.IsPending()) Row->Tex1.LoadSynchronous();
					Material[1]->SetTextureParameterValue("LipColor", Row->Tex1.Get());
					return true;
					*/
				}
			}
		}
	}
	return false;
}




void ARMCharacter::SetMorpherTargetValue(EMorphTarget Target, float Value)
{
	FPlayerCharacter& Me = GetMyPlayer();
	URMPlayerAnimInstance *AnimInst = GetAnimInst();
	if (AnimInst) {
		Me.MorphTargetValue[(int8)Target].Value = Value;
		AnimInst->SetMorphTarget(Me.MorphTargetValue[(int8)Target].Name,Value);
	}
}
//USkeletalMesh* NewMesh, UMaterialInstance* NewMaterial
void ARMCharacter::ChangeSkeletalMesh(FName Tag, USkeletalMesh* NewMesh, UMaterialInstance* NewMaterial)
{
	if (NewMesh == nullptr) {
		return;
	}
	TArray<UActorComponent*> Skel;//= GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	this->GetComponents(USkeletalMeshComponent::StaticClass(), Skel);
	for (auto It : Skel) {
		USkeletalMeshComponent* S = Cast<USkeletalMeshComponent>(It);
		if (S) {
			if (S->ComponentHasTag(Tag)) {
				S->SetSkeletalMesh(NewMesh);
				if (NewMaterial) {
					S->SetMaterial(0, NewMaterial);
					if (S->ComponentHasTag(FName(TEXT("Head"))))
						Material[1] = S->CreateDynamicMaterialInstance(0);
				}
			}
		}
	}
}

void ARMCharacter::UpdateLOD()
{
	if (!CustomLOD) return;
	FVector P1 = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetCameraLocation();
	FVector P2 = GetActorLocation();
	float Distance = (P1 - P2).Size();

	CurrentCustomLOD = 0;
	for (int32 I = 0; I < CustomLODDistanceRange.Num(); I++) {
		if (Distance > CustomLODDistanceRange[I]) {
			++CurrentCustomLOD;
		}
		else break;
	}
	CurrentCustomLOD = FMath::Clamp<float>(CurrentCustomLOD,
		(CustomLODLevelRange.X < -KINDA_SMALL_NUMBER) ? 0 : FMath::FloorToInt(CustomLODLevelRange.X),
		(CustomLODLevelRange.Y < -KINDA_SMALL_NUMBER) ? 0 : FMath::FloorToInt(CustomLODLevelRange.Y));
	
	USkeletalMeshComponent *SK = GetMesh();
	if (SK) {
		TArray<UActorComponent*> Skel = GetComponentsByClass(USkeletalMeshComponent::StaticClass());
		for (auto It : Skel) {
			USkeletalMeshComponent* S = Cast<USkeletalMeshComponent>(It);
			if (S) {
				S->SetForcedLOD(CurrentCustomLOD);
			}
		}
	}
}