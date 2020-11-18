// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CoverPoint.generated.h"

struct FCoverData
{
public:
	AActor* CoverObject;
	FVector Location;
	FVector PerpToWall;
	bool Occupied;

	FCoverData()
		: CoverObject(), Location(), PerpToWall(), Occupied(false)
	{}

	FCoverData(FVector _Location, FVector _PerpToWall)
		: CoverObject(), Location(_Location), PerpToWall(_PerpToWall), Occupied(false)
	{}
};

UCLASS()
class GAMEOFF_API UCoverPointObj : public UObject
{
	GENERATED_BODY()
public:

	UCoverPointObj();

	void InitCoverPoint(const FCoverPoint& CoverPoint);

	FVector Location;
	FVector PerpToWall;
	bool Occupied;
};

USTRUCT()
struct FCoverPoint 
{
public:	
	GENERATED_USTRUCT_BODY()
	/*Location of point*/
	FVector Location;

	/*Object that generated this cover point*/
	TWeakObjectPtr<AActor> CoverObject;

	/*Perpendicular vector to wall*/
	FVector PerpToWall;

	/*Is this point taken by pawn*/
	bool Occupied; 

	FCoverPoint() : 
		Location(), CoverObject(), PerpToWall(), Occupied(false)
	{}

	FCoverPoint(FVector Vec) : 	
		Location(Vec), CoverObject(), PerpToWall(), Occupied(false)
	{}

	FCoverPoint(FCoverData Data) :
		Location(Data.Location), CoverObject(Data.CoverObject), PerpToWall(Data.PerpToWall), Occupied(Data.Occupied)
	{}	

	void Init(const UCoverPointObj& CoverPointObj)
	{
		Location = CoverPointObj.Location;
		PerpToWall = CoverPointObj.PerpToWall;
		Occupied = CoverPointObj.Occupied;
	}

	void Init(const FVector& CoverLocation)
	{
		Location = CoverLocation;
		PerpToWall = FVector::ZeroVector;
		Occupied = false;
	}

};

