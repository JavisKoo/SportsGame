#include "CARPlaneActor.h"
#include "ProceduralMeshComponent.h"
#include "AugmentedReality/Public/ARTrackable.h"
#include "Components/InstancedStaticMeshComponent.h"
// Sets default values
ACARPlaneActor::ACARPlaneActor()
{
	PlanePolygonMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PlanePolygonMesh"));
	RootComponent = PlanePolygonMeshComponent;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called every frame
void ACARPlaneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlanePolygonMeshComponent->SetWorldTransform(ARCorePlaneObject->GetLocalToWorldTransform());

}

void ACARPlaneActor::UpdatePlanePolygonMesh()
{
	// Update polygon mesh vertex indices, using triangle fan due to its convex.
	TArray<FVector> BoundaryVertices;
	BoundaryVertices = ARCorePlaneObject->GetBoundaryPolygonInLocalSpace();
	int BoundaryVerticesNum = BoundaryVertices.Num();

	if (BoundaryVerticesNum < 3)
	{
		PlanePolygonMeshComponent->ClearMeshSection(0);
		return;
	}

	int PolygonMeshVerticesNum = BoundaryVerticesNum * 2;
	// Triangle number is interior(n-2 for convex polygon) plus perimeter (EdgeNum * 2);
	int TriangleNum = BoundaryVerticesNum - 2 + BoundaryVerticesNum * 2;

	TArray<FVector> PolygonMeshVertices;
	TArray<FLinearColor> PolygonMeshVertexColors;
	TArray<int> PolygonMeshIndices;
	TArray<FVector> PolygonMeshNormals;
	TArray<FVector2D> PolygonMeshUVs;

	PolygonMeshVertices.Empty(PolygonMeshVerticesNum);
	PolygonMeshVertexColors.Empty(PolygonMeshVerticesNum);
	PolygonMeshIndices.Empty(TriangleNum * 3);
	PolygonMeshNormals.Empty(PolygonMeshVerticesNum);


	FVector PlaneNormal = ARCorePlaneObject->GetLocalToWorldTransform().GetRotation().GetUpVector();
	for (int i = 0; i < BoundaryVerticesNum; i++)
	{
		FVector BoundaryPoint = BoundaryVertices[i];
		float BoundaryToCenterDist = BoundaryPoint.Size();
		float FeatheringDist = FMath::Min(BoundaryToCenterDist, EdgeFeatheringDistance);
		FVector InteriorPoint = BoundaryPoint - BoundaryPoint.GetUnsafeNormal() * FeatheringDist;

		PolygonMeshVertices.Add(BoundaryPoint);
		PolygonMeshVertices.Add(InteriorPoint);

		PolygonMeshUVs.Add(FVector2D(BoundaryPoint.X, BoundaryPoint.Y));
		PolygonMeshUVs.Add(FVector2D(InteriorPoint.X, InteriorPoint.Y));

		PolygonMeshNormals.Add(PlaneNormal);
		PolygonMeshNormals.Add(PlaneNormal);

		PolygonMeshVertexColors.Add(FLinearColor(0.0f, 0.f, 0.f, 0.f));
		PolygonMeshVertexColors.Add(FLinearColor(0.0f, 0.f, 0.f, 1.f));

	}

	// Generate triangle indices

	// Perimeter triangles
	for (int i = 0; i < BoundaryVerticesNum - 1; i++)
	{
		PolygonMeshIndices.Add(i * 2);
		PolygonMeshIndices.Add(i * 2 + 2);
		PolygonMeshIndices.Add(i * 2 + 1);

		PolygonMeshIndices.Add(i * 2 + 1);
		PolygonMeshIndices.Add(i * 2 + 2);
		PolygonMeshIndices.Add(i * 2 + 3);
	}

	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2);
	PolygonMeshIndices.Add(0);
	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2 + 1);


	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2 + 1);
	PolygonMeshIndices.Add(0);
	PolygonMeshIndices.Add(1);

	// interior triangles
	for (int i = 3; i < PolygonMeshVerticesNum - 1; i += 2)
	{
		PolygonMeshIndices.Add(1);
		PolygonMeshIndices.Add(i);
		PolygonMeshIndices.Add(i + 2);
	}

	// No need to fill uv and tangent;
	PlanePolygonMeshComponent->CreateMeshSection_LinearColor(0, PolygonMeshVertices, PolygonMeshIndices, PolygonMeshNormals, PolygonMeshUVs, PolygonMeshVertexColors, TArray<FProcMeshTangent>(), false);

}

