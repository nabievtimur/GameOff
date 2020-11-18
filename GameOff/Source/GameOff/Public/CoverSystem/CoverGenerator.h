// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "UObject/UObjectGlobals.h"
#include "CoverSystem/CoverPoint.h"
#include "CoverSystem/CoverOctree.h"
#include "CoverGenerator.generated.h"


UCLASS()
class GAMEOFF_API ACoverGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoverGenerator();

	// Called every frame
	virtual void Tick(float DeltaTime) override;	

	/*Generate cover points on map*/
	void GenerateCovers();

	/*Analize data providing by NavMesh*/
	void AnalizeNavData(FRecastDebugGeometry& NavMeshGeometry);	

	/*Process edge points*/
	void ProcessSegmentPoint(const FVector& SegmentPoint, const FVector& SegmentDir); 

	/*Analize point for providing cover*/
	bool IsProvidingCover(FCoverData& DataOut, const FVector& SegmentPoint, const FVector& SegmentDir);

	/*Hold cover*/
	bool OccupyCover(FCoverPoint& CoverPoint);

	/*Get coverpoints in area bounds*/
	void GetCoverWithinBounds(const FBoxCenterAndExtent& BoundsIn, TArray<FCoverPoint>& OutPoints);

	void GetPointsWithinBounds(const FBoxCenterAndExtent& BoundsIn, TArray<FNavLocation>& OutPoints);
	
	/*Release occupied cover*/
	bool ReleaseCover(FCoverPoint& CoverPoint);

	/*Get generator*/
	UFUNCTION(BlueprintPure, Category = "CoverGenerator", meta = (WorldContext = "WorldContextObject"))
	static ACoverGenerator* GetCoverGenerator(UObject* WorldContextObject);
 	
	/*Get nav data*/
	ARecastNavMesh* GetNavData(UWorld* World);

	/*Get trace vector*/
	FVector GetPerpProjectionXY(const FVector& Vec);

	/*Get navmesh data from the World*/
	ARecastNavMesh* GetNavMeshData(UWorld* World);

	/*Check for already generated points in area*/
	bool AnyConflictsWithinBounds(const FBoxCenterAndExtent& BoundsIn);

	/*Draw current octree*/
	void DrawOctreeBounds();

	UPROPERTY(Category = "Segment", EditAnywhere)
	float SegmentLength = 30.0f; 

	UPROPERTY(Category = "Segment", EditAnywhere)
	float GroundOffset = 20.0f;	

	UPROPERTY(Category = "Segment", EditAnywhere)
	float TraceTestLength = 60.0f;

	UPROPERTY(Category = "Segment", EditAnywhere)
	float MinSpaceBetweenPoints = 100.0f;

	UPROPERTY(Category = "Debug", EditAnywhere)
	float DebugDistance = 200.0f; 

	UPROPERTY(Category = "Debug", EditAnywhere)
	bool bDebug = true; 

private: 

	/*Start analizing mesh on navmesh generation finished*/
	UFUNCTION(CallInEditor)
	void OnNavmeshGenerationFinishedHandler(ANavigationData* NavData);

	TCoverOctree* CoverPointOctree;

	UPROPERTY(Transient)
	TArray<FCoverPoint> AllCoverPoints;

	// UPROPERTY(Transient)
	// TArray<FCoverPoint*> OccupiedCoverPoints;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
