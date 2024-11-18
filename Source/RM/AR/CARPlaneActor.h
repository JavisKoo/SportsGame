#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CARPlaneActor.generated.h"

UCLASS()
class RM_API ACARPlaneActor : public AActor
{
	GENERATED_BODY()
	//asefewafasdfasdf
	
public:
	// Sets default values for this actor's properties
	ACARPlaneActor();

	/* The procedural mesh component */
	UPROPERTY(Category = GoogleARCorePlaneActor, EditAnywhere, BlueprintReadWrite)
		class UProceduralMeshComponent* PlanePolygonMeshComponent;

	/** When set to true, the actor will remove the ARAnchor object from the current tracking session when the Actor gets destroyed.*/
	UPROPERTY(Category = GoogleARCorePlaneActor, BlueprintReadWrite)
		class UARPlaneGeometry* ARCorePlaneObject = nullptr;

	/** The feathering distance for the polygon edge. Default to 10 cm*/
	UPROPERTY(Category = GoogleARCorePlaneActor, EditAnywhere, BlueprintReadWrite)
		float EdgeFeatheringDistance = 10.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "GoogleARCorePlaneActor", meta = (Keywords = "googlear arcore plane"))
		void UpdatePlanePolygonMesh();

};
