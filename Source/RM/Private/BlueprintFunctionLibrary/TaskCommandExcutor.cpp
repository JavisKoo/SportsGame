#include "BlueprintFunctionLibrary/TaskCommandExcutor.h"
#include "Task/TaskCommand.h"
#include "Kismet/GameplayStatics.h"

void UTaskCommandExcutor::SpawnPlayer(const UObject* WorldContextObject)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISpawnPlayer(A);
	}
}

void UTaskCommandExcutor::Rand(const UObject* WorldContextObject, ERandType RandType)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRand(A, RandType);
	}
}
/*
void UTaskCommandExcutor::RandMorphTargetValue(const UObject* WorldContextObject, EMorphGroup Group)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRandMorpherTargetValue(A,Group);
	}
}

void UTaskCommandExcutor::RandHead(const UObject* WorldContextObject)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRandChangeHead(A);
	}

	
}

void UTaskCommandExcutor::RandFaceSkin(const UObject* WorldContextObject)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRandChangeFaceSkin(A);
	}
}

void UTaskCommandExcutor::RandSkinColor(const UObject* WorldContextObject)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRandChangeSkinColor(A);
	}
}

void UTaskCommandExcutor::RandHandScale(const UObject* WorldContextObject)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRandHandScale(A);
	}
}

void UTaskCommandExcutor::RandScale(const UObject* WorldContextObject)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRandScale(A);
	}
}
void UTaskCommandExcutor::RandHeadScale(const UObject* WorldContextObject)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_IRandHeadScale(A);
	}
}

*/

void UTaskCommandExcutor::SetGender(const UObject* WorldContextObject, EGenderRM Gender)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out)
	{
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetGender(A, Gender);
	}
}


void UTaskCommandExcutor::SetScale(const UObject* WorldContextObject, float Scale)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetScale(A, Scale);
	}
}


void UTaskCommandExcutor::SetSkinColor(const UObject* WorldContextObject, FLinearColor SkinColor)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetSkinColor(A, SkinColor);
	}
}

void UTaskCommandExcutor::SetHeadScale(const UObject* WorldContextObject, float Scale)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetHeadScale(A, Scale);
	}
}

void UTaskCommandExcutor::SetHandScale(const UObject* WorldContextObject, float Scale)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetHandScale(A,Scale);
	}
}

void UTaskCommandExcutor::SetTopSize(const UObject* WorldContextObject, float Value)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetTopSize(A, Value);
	}
}

void UTaskCommandExcutor::SetCheekSize(const UObject* WorldContextObject, float Size)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetCheekSize(A, Size);
	}

}

void UTaskCommandExcutor::SetBottomSize(const UObject* WorldContextObject, float Size)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetBottomSize(A, Size);
	}

}
void UTaskCommandExcutor::SetCostumePart(const UObject* WorldContextObject, ECostumePart Part, FName RowName)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetCostumePart(A, Part,RowName);
	}
}

void UTaskCommandExcutor::SetMorperTargetValue(const UObject* WorldContextObject, EMorphTarget Target, float Value)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetMorpherTargetValue(A, Target, Value);
	}

}

void UTaskCommandExcutor::SetEyeMaterialScalarParameter(const UObject* WorldContextObject, EEyeMaterialScalarParameter Enum, float Value)
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, UTaskCommand::StaticClass(), Out);
	for (AActor* A : Out) {
		ITaskCommand* Task = Cast<ITaskCommand>(A);
		Task->Execute_ISetEyeMaterialScalarParameter(A, Enum, Value);
	}
}
