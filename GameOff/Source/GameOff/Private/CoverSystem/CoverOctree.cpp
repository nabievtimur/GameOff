#include "CoverSystem/CoverOctree.h"

TCoverOctree::TCoverOctree()
	: TOctree<FCoverPointOctreeElement, FCoverPointOctreeSemantics>()
{}

TCoverOctree::TCoverOctree(const FVector& Origin, float Radius)
	: TOctree<FCoverPointOctreeElement, FCoverPointOctreeSemantics>(Origin, Radius)
{}

TCoverOctree::~TCoverOctree()
{}

bool TCoverOctree::AddCoverPoint(FCoverData& CoverData, const float DuplicateRadius)
{
	/* check if any cover points are close enough - if so, abort */
	if (AnyCoverPointsWithinBounds(FBoxCenterAndExtent(CoverData.Location, FVector(DuplicateRadius)).GetBox()))
		return false;

 //    FVector Loc = CoverPoint.Location;
	// UE_LOG(LogTemp, Warning, TEXT("Cover Loc %s"), *Loc.ToString());
	
	AddElement(FCoverPointOctreeElement(CoverData));
	return true;
}

bool TCoverOctree::AnyCoverPointsWithinBounds(const FBox& QueryBox) const
{
	return (TCoverOctree::TConstElementBoxIterator<> (*this, QueryBox)).HasPendingElements();
}

void TCoverOctree::FindCoverPoints(TArray<FCoverPoint>& OutCoverPoints, const FBox& QueryBox) const
{
	for (TCoverOctree::TConstElementBoxIterator<> It(*this, QueryBox); It.HasPendingElements(); It.Advance())
		OutCoverPoints.Add(*(It.GetCurrentElement().CoverPoint.Get()));
}

void TCoverOctree::SetElementId(const FVector Location, FOctreeElementId Id)
{
	ElementToID.Add(Location, Id);
}

bool TCoverOctree::GetElementID(FOctreeElementId& OutElementID, const FVector ElementLocation) const
{
	const FOctreeElementId* element = ElementToID.Find(ElementLocation);
	if (!element || !element->IsValidId())
		return false;

	OutElementID = *element;
	return true;
}

bool TCoverOctree::OccupyCover(FOctreeElementId ElementID)
{
	if (!ElementID.IsValidId())
		return false;

	FCoverPointOctreeElement CoverElement = GetElementById(ElementID);
	if (!CoverElement.CoverPoint->Occupied)
	{
		CoverElement.CoverPoint->Occupied = true;
		return true;
	}
	
	return false;
}

bool TCoverOctree::ReleaseCover(FOctreeElementId ElementID)
{
	if (!ElementID.IsValidId())
		return false;

	FCoverPointOctreeElement CoverElement = GetElementById(ElementID);
	if (CoverElement.CoverPoint->Occupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cover released"));
		CoverElement.CoverPoint->Occupied = false;
		return true;
	}
	
	return false;
}

/* -----------------------------
		   semantics
   ---------------------------- */

void FCoverPointOctreeSemantics::SetElementId(FCoverPointOctreeSemantics::FOctree& OctreeOwner, 
											 const FCoverPointOctreeElement& Element, FOctreeElementId Id)
{
	((TCoverOctree&)OctreeOwner).SetElementId(Element.CoverPoint->Location, Id);
}
