// Fill out your copyright notice in the Description page of Project Settings.

#include "TeleportArea.h"
#include "ProceduralMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "ConstructorHelpers.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATeleportArea::ATeleportArea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	pProceduralMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComp"));
	pSplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	pInstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("OutlineMesh"));

	pProceduralMeshComp->bUseAsyncCooking = true;
	ClearSpline();

	pProceduralMeshComp->SetupAttachment(RootComponent);
	pSplineComp->SetupAttachment(RootComponent);
	pInstancedStaticMesh->SetupAttachment(RootComponent);

	if(pProceduralMeshMaterial == nullptr)
	{
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> foundMaterial(TEXT("/Game/Materials/Teleportation/M_TeleportFlat.M_TeleportFlat"));
		if (foundMaterial.Succeeded())
			pProceduralMeshMaterial = foundMaterial.Object;
	}

	if(pInstancedStaticMesh == nullptr)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> foundMesh(TEXT("/Game/Art_Assets/BasicShapes/SM_Plane.SM_Plane"));
		if (foundMesh.Succeeded())
			pInstancedStaticMesh->SetStaticMesh(foundMesh.Object);
	}
}

// Called when the game starts or when spawned
void ATeleportArea::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATeleportArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	bTeleportedTo = false;

	if(bIsEnabled)
	{
		if(pProceduralMeshComp != nullptr)
			pProceduralMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}else
	{
		if(pProceduralMeshComp != nullptr)
			pProceduralMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ATeleportArea::OnConstruction(const FTransform& Transform)
{
	CreateTeleportationArea();

	for (int i = 0; i < pSplineComp->GetNumberOfSplinePoints(); i++)
	{
		pSplineComp->SetSplinePointType(i, ESplinePointType::Linear, false);
	}
}

void ATeleportArea::CreateTeleportationArea()
{
	VisualizationTriangles.Reset(0);
	TriangulateSpline();
	CreateVisualisationMesh();
	CreateVisualizationTeleportArea();
}

void ATeleportArea::ClearSpline()
{
	pSplineComp->ClearSplinePoints(true);

	TArray<FSplinePoint> splinePoints;
	splinePoints.Add(FSplinePoint(0, FVector(-StandardSize, -StandardSize, 0.f)));
	splinePoints.Add(FSplinePoint(1, FVector(StandardSize, -StandardSize, 0.f)));
	splinePoints.Add(FSplinePoint(2, FVector(StandardSize, StandardSize, 0.f)));
	splinePoints.Add(FSplinePoint(3, FVector(-StandardSize, StandardSize, 0.f)));

	pSplineComp->AddPoints(splinePoints, false);
	for (int i = 0; i < pSplineComp->GetNumberOfSplinePoints(); i++)
	{
		pSplineComp->SetSplinePointType(i, ESplinePointType::Linear, false);
	}

	pSplineComp->SetClosedLoop(true);
}

inline int CircularIndex(int index, int length)
{
	int temp = index % length;
	int temp1 = temp + length;
	if (temp < 0)
		return temp1;

	return temp;
}

inline bool PointIsConvex(FVector prevPoint, FVector curPoint, FVector nextPoint)
{
	FVector temp1 = prevPoint - curPoint;
	FVector temp2 = nextPoint - curPoint;
	temp1.Normalize();
	temp2.Normalize();
	FVector temp3 = FVector::CrossProduct(temp1, temp2);

	return temp3.Z < 0;
}

inline bool IsPointInTriangle(FVector t1, FVector t2, FVector t3, FVector p)
{
	//Get two edges of the triangle and the line to the point
	FVector v1 = t3 - t1;
	FVector v2 = t2 - t1;
	FVector v3 = p - t1;

	//Convert the triangle edges to a Barycentric Coordinate System
	float daa = FVector::DotProduct(v1, v1);
	float dab = FVector::DotProduct(v1, v2);
	float dac = FVector::DotProduct(v1, v3);

	float dbb = FVector::DotProduct(v2, v2);
	float dbc = FVector::DotProduct(v2, v3);

	float u = (((dbb * dac) - (dab * dbc)) / ((daa * dbb) - (dab * dab)));
	float v = (((daa * dbc) - (dab * dac)) / ((daa * dbb) - (dab * dab)));

	//If the position of the point in the barycentric coord sys is greater than 1 is is not inside the triangle
	return (v >= 0.f) && (u >= 0.f) && ((u + v) < 1.f);
}

inline bool IsPointAnEar(int curPointIndex, int prevPointIndex, int nextPointIndex, TArray<int> reflexIndices, TArray<FVector> splinePoints)
{
	//Check if any reflex points are inside the triangle created by the 3 points if so it is an ear
	bool IsEar = true;

	FVector curPoint = splinePoints[curPointIndex];
	FVector prevPoint = splinePoints[prevPointIndex];
	FVector nextPoint = splinePoints[nextPointIndex];
	for (int i = 0; i < reflexIndices.Num(); i++)
	{
		FVector tempPoint = splinePoints[reflexIndices[i]];
		IsEar = !IsPointInTriangle(curPoint, prevPoint, nextPoint, tempPoint);
		if (IsEar == false)
			break;
	}
	return IsEar;
}

TArray<MeshTriangle> ATeleportArea::TriangulateSpline()
{
	TArray<FVector> splinePoints = GetSplinePoints();
	TArray<FVector> reflexPoints = TArray<FVector>{};
	TArray<FVector> convexPoints = TArray<FVector>{};
	TArray<FVector> earPoints = TArray<FVector>{};
	TArray<int> reflexIndices = TArray<int>{};
	TArray<int> convexIndices = TArray<int>{};
	TArray<int> earIndices = TArray<int>{};

	GetPolygonComponents(splinePoints, reflexPoints, convexPoints, earPoints, reflexIndices, convexIndices, earIndices);
	reflexPoints.Reset();
	convexPoints.Reset();
	earPoints.Reset();
	return GetTrianglesFromPoints(splinePoints, convexIndices, reflexIndices, earIndices);
}

void ATeleportArea::GetPolygonComponents(TArray<FVector> splinePoints, TArray<FVector>& reflexPoints, TArray<FVector>& convexPoints, TArray<FVector>& earPoints, TArray<int>& reflexIndices, TArray<int>& convexIndices, TArray<int>& earIndices)
{
	{
		FVector prevPoint = FVector();
		FVector nextPoint = FVector();
		for (int curIndex = 0; curIndex < splinePoints.Num(); curIndex++)
		{
			FVector curPoint = splinePoints[curIndex];
			int prevIndex = CircularIndex(curIndex - 1, splinePoints.Num());
			prevPoint = splinePoints[prevIndex];
			int nextIndex = CircularIndex(curIndex + 1, splinePoints.Num());
			nextPoint = splinePoints[nextIndex];

			if (PointIsConvex(prevPoint, curPoint, nextPoint))
			{
				convexPoints.Add(curPoint);
				convexIndices.Add(curIndex);
			}
			else
			{
				reflexPoints.Add(curPoint);
				reflexIndices.Add(curIndex);
			}
		}
	}

	//Test to see if point is an ear
	{
		for (int curIndex = 0; curIndex < convexIndices.Num(); curIndex++)
		{
			int curElement = convexIndices[curIndex];

			int prevIndex = CircularIndex(curElement - 1, splinePoints.Num());
			int nextIndex = CircularIndex(curElement + 1, splinePoints.Num());

			if (IsPointAnEar(curElement, prevIndex, nextIndex, reflexIndices, splinePoints))
			{
				earPoints.Add(splinePoints[curElement]);
				earIndices.Add(curElement);
			}
		}
	}
}

TArray<MeshTriangle> ATeleportArea::GetTrianglesFromPoints(TArray<FVector> splinePoints, TArray<int> convexIndices, TArray<int> reflexIndices, TArray<int> earIndices)
{
	int curPoint = 0;
	if (earIndices.Num() > 0)
		curPoint = earIndices[0];
	int prevPoint = CircularIndex(curPoint - 1, splinePoints.Num());
	int nextPoint = CircularIndex(curPoint + 1, splinePoints.Num());

	if(splinePoints.Num() <= 3)
	{
		MeshTriangle triangle;
		triangle.point1 = splinePoints[0];
		triangle.point2 = splinePoints[1];
		triangle.point3 = splinePoints[2];
		VisualizationTriangles.Add(triangle);
	}else
	{
		//Make a triangle of the ear point and its adjacent points
		MeshTriangle triangle;
		triangle.point1 = splinePoints[curPoint];
		triangle.point2 = splinePoints[prevPoint];
		triangle.point3 = splinePoints[nextPoint];

		//Remove the ear
		splinePoints.RemoveAt(curPoint);

		//Recalcualte the polygon components with the ear missing for an optimisation a linked list data structure can be used instead of recalculating at every step
		TArray<FVector> reflexPoints = TArray<FVector>{};
		TArray<FVector> convexPoints = TArray<FVector>{};
		TArray<FVector> earPoints = TArray<FVector>{};
		TArray<int> reflexIndices = TArray<int>{};
		TArray<int> convexIndices = TArray<int>{};
		TArray<int> earIndices = TArray<int>{};
		GetPolygonComponents(splinePoints, reflexPoints, convexPoints, earPoints, reflexIndices, convexIndices, earIndices);
		//TArray<MeshTriangle> trianglesRecursion = 
		GetTrianglesFromPoints(splinePoints, convexIndices, reflexIndices, earIndices);
		VisualizationTriangles.Add(triangle);
	}

	return VisualizationTriangles;
}

TArray<FVector> ATeleportArea::GetSplinePoints()
{
	TArray<FVector> splinePoints;
	for (int i = 0; i < pSplineComp->GetNumberOfSplinePoints(); i++)
	{
		splinePoints.Add(pSplineComp->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local));
	}
	return splinePoints;
}

void ATeleportArea::TrianglesToIndices(TArray<MeshTriangle> triangles, TArray<FVector>& vertices, TArray<int>& indices)
{
	for (int i = 0; i < triangles.Num(); i++)
	{
		//Create an array of individual triangle vertices
		int newIndex = vertices.AddUnique(triangles[i].point1);
		if (newIndex < 0)
			indices.Add(newIndex);
		else
			indices.Add(vertices.Find(triangles[i].point1));

		vertices.AddUnique(triangles[i].point2);
		if (newIndex < 0)
			indices.Add(newIndex);
		else
			indices.Add(vertices.Find(triangles[i].point2));

		vertices.AddUnique(triangles[i].point3);
		if (newIndex < 0)
			indices.Add(newIndex);
		else
			indices.Add(vertices.Find(triangles[i].point3));
	}
}

void ATeleportArea::CreateVisualisationMesh()
{
	TArray<FVector> vertices;
	TArray<int> indices;
	TrianglesToIndices(VisualizationTriangles, vertices, indices);

	TArray<FVector> normals;
	for(int i = 0; i < vertices.Num(); i++)
	{
		normals.Add(FVector(0, 0, 1));
	}
	TArray<FVector2D> uv;
	for (int i = 0; i < vertices.Num(); i++)
	{
		uv.Add(FVector2D(0, 0));
	}
	TArray<FProcMeshTangent> tangents;
	for (int i = 0; i < vertices.Num(); i++)
	{
		tangents.Add(FProcMeshTangent(0, 0, 1));
	}
	TArray<FColor> vertexColors;
	for (int i = 0; i < vertices.Num(); i++)
	{
		vertexColors.Add(FColor(0.75, 0.75, 0.75, 1.0));
	}

	pProceduralMeshComp->ClearMeshSection(0);
	pProceduralMeshComp->CreateMeshSection(0, vertices, indices, normals, uv, vertexColors, tangents, true);
	pProceduralMeshComp->SetMaterial(0, pProceduralMeshMaterial);
}

void ATeleportArea::CreateVisualizationTeleportArea()
{
	pInstancedStaticMesh->ClearInstances();
	int instanceCount = pSplineComp->GetNumberOfSplinePoints();
	for (int i = 0; i < instanceCount; i++)
	{
		FVector firstPoint = pSplineComp->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector secondPoint = pSplineComp->GetLocationAtSplinePoint(CircularIndex(i + 1, pSplineComp->GetNumberOfSplinePoints()), ESplineCoordinateSpace::World);
		FRotator rotationToPoint = UKismetMathLibrary::FindLookAtRotation(firstPoint, secondPoint);
		float length = FVector::Dist(firstPoint, secondPoint);

		FTransform newTransform;
		newTransform.SetLocation(FMath::Lerp(firstPoint, secondPoint, 0.5f));
		newTransform.SetRotation(FQuat(FRotator(rotationToPoint.Pitch, rotationToPoint.Yaw, 1.f)));
		newTransform.SetScale3D(FVector(length, EdgeWidth, 0.f));

		pInstancedStaticMesh->AddInstanceWorldSpace(newTransform);
	}
}

void ATeleportArea::SetVisible(bool newState)
{
	if (pProceduralMeshComp == nullptr)
		return;
	if (pInstancedStaticMesh == nullptr)
		return;

	if (bIsEnabled)
	{
		pProceduralMeshComp->SetVisibility(newState, true);
		pInstancedStaticMesh->SetVisibility(newState, true);
	}
	else
	{
		pProceduralMeshComp->SetVisibility(false, true);
		pInstancedStaticMesh->SetVisibility(false, true);
	}
}

