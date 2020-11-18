// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverSystem/CoverGenerator.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


// Sets default values
ACoverGenerator::ACoverGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	/*Scene root component*/
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	/*Our struct with coper points*/
	CoverPointOctree = new TCoverOctree(FVector(0, 0, 0), 64000);
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()); 
	if (NavSystem) 
	{
		GLog->Log("Nav system found");
		NavSystem->OnNavigationGenerationFinishedDelegate.AddDynamic(this, &ACoverGenerator::OnNavmeshGenerationFinishedHandler);
	}
}

// Called when the game starts or when spawned
void ACoverGenerator::BeginPlay()
{
	Super::BeginPlay();

	GenerateCovers();
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()); 
	if (NavSystem) 
	{
		GLog->Log("Nav system found");
		NavSystem->OnNavigationGenerationFinishedDelegate.AddDynamic(this, &ACoverGenerator::OnNavmeshGenerationFinishedHandler);
	}
}

void ACoverGenerator::OnNavmeshGenerationFinishedHandler(ANavigationData* NavData)
{
	GLog->Log("Nav system finished generating");
	GenerateCovers();
#if WITH_EDITOR
	DrawOctreeBounds();
#endif
}

// Called every frame
void ACoverGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ARecastNavMesh* ACoverGenerator::GetNavData(UWorld* World)
{
	const UNavigationSystemBase* NavSys = World->GetNavigationSystem();

	if (NavSys == nullptr)
	{
		return nullptr;
	}

	return Cast<ARecastNavMesh>(NavSys->GetMainNavData());
}

void ACoverGenerator::GenerateCovers() 
{
	GLog->Log("COVERS GENERATING");

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	ARecastNavMesh* NavMeshData = GetNavData(World);

	if (NavMeshData == nullptr)
	{
		GLog->Log("Cover generator: No nav mesh");
		return;
	}

	/*Set gathering edges*/
	FRecastDebugGeometry NavMeshGeometry;
	NavMeshGeometry.bGatherNavMeshEdges = true;
	NavMeshData->GetDebugGeometry(NavMeshGeometry);

	AnalizeNavData(NavMeshGeometry);
}

void ACoverGenerator::AnalizeNavData(FRecastDebugGeometry& NavMeshGeometry)
{
	const TArray<FVector>& NavMeshEdgeVerts = NavMeshGeometry.NavMeshEdges;

	for (int32 i = 0; i < NavMeshEdgeVerts.Num(); i += 2)
	{
		const FNavLocation SegmentStart = FNavLocation(NavMeshEdgeVerts[i]);
		const FNavLocation SegmentEnd = FNavLocation(NavMeshEdgeVerts[i + 1]);

		const FVector Segment = SegmentEnd.Location - SegmentStart.Location;
		const float SegmentSize = Segment.Size();
		//FVector Perp = Get2DPerpVector(Segment);
		FVector SegmentDir = Segment;
		SegmentDir.Normalize();

		// Check start and end position 			

		const FVector GroundOffsetV(0.0f, 0.0f, GroundOffset);

		// Check inner points if segment is long enough 
 		if (SegmentSize > SegmentLength)
		{
			ProcessSegmentPoint(SegmentStart.Location + GroundOffsetV, SegmentDir);
			ProcessSegmentPoint(SegmentEnd.Location + GroundOffsetV, SegmentDir);

#if WITH_EDITOR
	 		if (bDebug && FVector::Dist(GetActorLocation(), SegmentStart.Location) < DebugDistance)
	 		{
	 			DrawDebugSphere(GetWorld(), SegmentStart.Location + GroundOffsetV, 20, 4, FColor::Green, true, 10.f);
	 			DrawDebugSphere(GetWorld(), SegmentEnd.Location + GroundOffsetV, 20, 4, FColor::Red, true, 10.f);
	 		}
#endif
			const int32 NumSegmentPieces = Segment.Size() / SegmentLength;
			const float RealSegmentLength = (float)Segment.Size() / (float)NumSegmentPieces;
			for (int32 j = 1; j < NumSegmentPieces; j++)
			{
				const FVector SegmentPoint = SegmentStart.Location + (j * RealSegmentLength * SegmentDir) + GroundOffsetV;
				ProcessSegmentPoint(SegmentPoint, SegmentDir);

#if WITH_EDITOR
				if (bDebug && FVector::Dist(GetActorLocation(), SegmentPoint) < DebugDistance)
				{
					DrawDebugSphere(GetWorld(), SegmentPoint, 20, 4, FColor::Blue, true, 10.f);
				}
#endif
			}
		}
		else 
		{
			const FVector MidPoint = SegmentStart.Location + SegmentLength / 2 * SegmentDir + GroundOffsetV;
			ProcessSegmentPoint(MidPoint, SegmentDir);
#if WITH_EDITOR
			if (bDebug && FVector::Dist(GetActorLocation(),MidPoint) < DebugDistance)
			{
				DrawDebugSphere(GetWorld(), MidPoint, 20, 4, FColor::Blue, true, 10.f);
			}
#endif			
		}
	}
	
	CoverPointOctree->ShrinkElements();
}

FVector ACoverGenerator::GetPerpProjectionXY(const FVector& Vec)
{
	return FVector(Vec.Y, -Vec.X, 0);
}

bool ACoverGenerator::IsProvidingCover(FCoverData& DataOut, const FVector& SegmentPoint, const FVector& SegmentDir)
{
	FVector TraceDir = GetPerpProjectionXY(SegmentDir);
	TraceDir.Normalize();

	const FVector TraceStart = SegmentPoint;
	const FVector TraceEndPos = SegmentPoint + TraceDir * TraceTestLength; 
	const FVector TraceEndNeg = SegmentPoint - TraceDir * TraceTestLength; 
	const FVector GroundOffsetV(0.0f, 0.0f, GroundOffset);

	FHitResult HitResult;
	FCollisionQueryParams CollQueryParams;
	CollQueryParams.TraceTag = "CoverGenerator_ScanForCoverNavMeshProjection";

	DataOut.Location = SegmentPoint;

	bool bSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart + GroundOffsetV, TraceEndPos + GroundOffsetV, ECollisionChannel::ECC_GameTraceChannel1, CollQueryParams);

	if (!bSuccess)
	{
		bSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart + GroundOffsetV, TraceEndNeg + GroundOffsetV, ECollisionChannel::ECC_GameTraceChannel1, CollQueryParams);
		DataOut.PerpToWall = -(TraceDir * TraceTestLength);
		DataOut.PerpToWall.Normalize();

		//DrawDebugDirectionalArrow(GetWorld(), DataOut.Location, DataOut.Location + DataOut.PerpToWall, 100.0f, FColor::Purple, true, -1.0f, 0, 2.0f);
		return bSuccess;
	}

	DataOut.PerpToWall = TraceDir * TraceTestLength;
	DataOut.PerpToWall.Normalize();

	return bSuccess;
}

void ACoverGenerator::ProcessSegmentPoint(const FVector& SegmentPoint, const FVector& SegmentDir) 
{
	FCoverData CoverData;
	if(IsProvidingCover(CoverData, SegmentPoint, SegmentDir)) 
	{
		/*Add point if there's no other points close enough*/
		//DrawDebugDirectionalArrow(GetWorld(), CoverData.Location, CoverData.PerpToWall * 20.0f, 100.0f, FColor::Purple, true, -1.0f, 0, 2.0f);
		CoverPointOctree->AddCoverPoint(CoverData, MinSpaceBetweenPoints); 
	}
}

void ACoverGenerator::GetCoverWithinBounds(const FBoxCenterAndExtent& BoundsIn, TArray<FCoverPoint>& OutPoints)
{
	// Iterating over a region in the octree and storing the elements

	for (TCoverOctree::TConstElementBoxIterator<> OctreeIt(*CoverPointOctree, BoundsIn);
		OctreeIt.HasPendingElements();	
		OctreeIt.Advance())
    {
    	FCoverPoint CoverPoint = *(OctreeIt.GetCurrentElement().CoverPoint.Get());
  //   	FVector Loc = CoverPoint.Location;
		// UE_LOG(LogTemp, Warning, TEXT("Cover Loc %s"), *Loc.ToString());
		OutPoints.Add(CoverPoint);
	}
}

void ACoverGenerator::GetPointsWithinBounds(const FBoxCenterAndExtent& BoundsIn, TArray<FNavLocation>& OutPoints)
{
	// Iterating over a region in the octree and storing the elements

	for (TCoverOctree::TConstElementBoxIterator<> OctreeIt(*CoverPointOctree, BoundsIn);
		OctreeIt.HasPendingElements();	
		OctreeIt.Advance())
    {
    	FCoverPoint CoverPoint = *(OctreeIt.GetCurrentElement().CoverPoint.Get());
    	FVector Loc = CoverPoint.Location;
		UE_LOG(LogTemp, Warning, TEXT("Cover Loc %s"), *Loc.ToString());
		OutPoints.Add(FNavLocation(CoverPoint.Location));
	}
}

ACoverGenerator* ACoverGenerator::GetCoverGenerator(UObject* WorldContextObject)
{
	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		TActorIterator<ACoverGenerator> Itr(WorldContextObject->GetWorld());
		if (Itr)
		{
			return *Itr;
		}
	}
	return NULL;
}

void ACoverGenerator::DrawOctreeBounds()
{
	if (!GetWorld())
		return; 

	FVector extent = CoverPointOctree->GetRootBounds().Extent;

	float max = extent.GetMax();
	FVector maxExtent = FVector(max, max, max);
	FVector center = CoverPointOctree->GetRootBounds().Center;

	//GLog->Log("DrawingOctree");
	DrawDebugBox(GetWorld(), center, maxExtent, FColor().Blue, true, 1.0f);
	DrawDebugSphere(GetWorld(), center + maxExtent, 4.0f, 12, FColor().White, true, 1.0f);
	DrawDebugSphere(GetWorld(), center - maxExtent, 4.0f, 12, FColor().White, true, 1.0f);
}

bool ACoverGenerator::OccupyCover(FCoverPoint& CoverPoint) 
{
	FOctreeElementId CoverId;
	if (CoverPointOctree->GetElementID(CoverId, CoverPoint.Location))
	{
		if(CoverPointOctree->OccupyCover(CoverId))
		{
			return true;
		}
	}
	return false;
}

bool ACoverGenerator::ReleaseCover(FCoverPoint& CoverPoint)
{
	FOctreeElementId CoverId;
	if (CoverPointOctree->GetElementID(CoverId, CoverPoint.Location))
	{
		if(CoverPointOctree->ReleaseCover(CoverId))
		{
			return true;
		}
	}
	return false;
}