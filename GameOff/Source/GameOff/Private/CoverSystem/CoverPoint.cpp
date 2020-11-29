
#include "CoverSystem/CoverPoint.h"

UCoverPointObj::UCoverPointObj()
	: Location(FVector::ZeroVector), PerpToWall(FVector::ZeroVector), Occupied(false)
{}	

void UCoverPointObj::InitCoverPoint(const FCoverPoint& CoverPoint)
{
	Location = CoverPoint.Location;
	PerpToWall = CoverPoint.PerpToWall;
	Occupied = CoverPoint.Occupied;
}