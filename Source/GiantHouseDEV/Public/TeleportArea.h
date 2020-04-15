// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "ProceduralMeshComponent.h"
#include "TeleportArea.generated.h"

class USplineComponent;
class UProceduralMeshComponent;
class UMaterialInterface;
class UInstancedStaticMeshComponent;

struct MeshTriangle
{
	FVector point1 = FVector();
	FVector point2 = FVector();
	FVector point3 = FVector();
};

UCLASS()
class GIANTHOUSEDEV_API ATeleportArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform & Transform);

	void CreateTeleportationArea();

	//Set's up 4 spline points in a rectangle form with linear curves in a closed loop
	void ClearSpline();
	TArray<MeshTriangle> TriangulateSpline();
	void GetPolygonComponents(TArray<FVector> splinePoints, TArray<FVector>& reflexPoints, TArray<FVector>& convexPoints, TArray<FVector>& earPoints, TArray<int>& reflexIndices, TArray<int>& convexIndices, TArray<int>& earIndices);
	TArray<MeshTriangle> GetTrianglesFromPoints(TArray<FVector> splinePoints, TArray<int> convexIndices, TArray<int> reflexIndices, TArray<int> earIndices);
	TArray<FVector> GetSplinePoints();
	void TrianglesToIndices(TArray<MeshTriangle> triangles, TArray<FVector>& vertices, TArray<int>& indices);
	void CreateVisualisationMesh();
	void CreateVisualizationTeleportArea();

	void SetVisible(bool newState);

	TArray<MeshTriangle> VisualizationTriangles;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	USplineComponent* pSplineComp = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UInstancedStaticMeshComponent* pInstancedStaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Default)
	UMaterialInterface* pProceduralMeshMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Default)
	float EdgeWidth = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		float StandardSize = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		bool bIsEnabled = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
		UProceduralMeshComponent * pProceduralMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bAbleToTeleport = true;

	bool bTeleportedTo = false;
	
};
