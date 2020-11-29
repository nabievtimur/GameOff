
#include "CoreMinimal.h"
#include "CoverSystem/CoverPoint.h"
#include "Math/GenericOctree.h"
#include "Math/GenericOctreePublic.h"
#include "Runtime/Core/Public/Math/GenericOctree.h"

struct FCoverPointOctreeElement
{
	/** Cover Point these data represents */
	TSharedPtr<FCoverPoint, ESPMode::ThreadSafe> CoverPoint;

	FBoxSphereBounds Bounds;

	FCoverPointOctreeElement(FCoverPoint& CoverPointIn) : 
						CoverPoint(&CoverPointIn), Bounds(FSphere(CoverPointIn.Location, 1.0f))
	{}

	FCoverPointOctreeElement(FCoverData& Data) :
						CoverPoint(new FCoverPoint(Data)), Bounds(FSphere(Data.Location, 1.0f))
	{}

	FORCEINLINE UObject* GetOwner() const { return CoverPoint->CoverObject.Get(); }
};

struct FCoverPointOctreeSemantics
{	
	typedef TOctree<FCoverPointOctreeElement, FCoverPointOctreeSemantics> FOctree;
	enum { MaxElementsPerLeaf = 16 };
	enum { MinInclusiveElementsPerNode = 7 };
	enum { MaxNodeDepth = 12 };

	/** Using the heap allocator instead of an inline allocator to trade off add/remove performance for memory. */
	/** Since we won't generate covers after init, should be ok. */
	typedef FDefaultAllocator ElementAllocator;

	FORCEINLINE 
	static bool AreElementsEqual(const FCoverPointOctreeElement& A, const FCoverPointOctreeElement& B)
	{
		return A.CoverPoint == B.CoverPoint;
	}

	FORCEINLINE 
	static const FBoxSphereBounds& GetBoundingBox(const FCoverPointOctreeElement& CoverData)
	{
		return CoverData.Bounds;
	}

	FORCEINLINE
	static void SetElementId(FOctree& OctreeOwner, const FCoverPointOctreeElement& Element, FOctreeElementId Id);
};

//typedef TOctree<FCoverPointOctreeElement, FCoverPointOctreeSemantics> FCoverPointOctree;

class TCoverOctree : public TOctree<FCoverPointOctreeElement, FCoverPointOctreeSemantics>, public TSharedFromThis<TCoverOctree, ESPMode::ThreadSafe>
{
public:
	TCoverOctree();

	TCoverOctree(const FVector& Origin, float Radius);

	virtual ~TCoverOctree();

	TMap<const FVector, FOctreeElementId> ElementToID;

	/* Adds a cover point to the octree */
	bool AddCoverPoint(FCoverData& CoverData, const float DuplicateRadius);

	// Checks if any cover points are within the supplied bounds.
	bool AnyCoverPointsWithinBounds(const FBox& QueryBox) const;

	// Finds cover points that intersect the supplied box.
	void FindCoverPoints(TArray<FCoverPoint>& OutCoverPoints, const FBox& QueryBox) const;

	// Finds cover points that intersect the supplied sphere.
	void FindCoverPoints(TArray<FCoverPointOctreeElement>& OutCoverPoints, const FSphere& QuerySphere) const;

	// Won't crash the game if ElementID is invalid, unlike the similarly named superclass method. This method hides the base class method as it's not virtual.
	void RemoveElement(FOctreeElementId ElementID);

	/*Get ID by point location*/
	bool GetElementID(FOctreeElementId& OutElementID, const FVector ElementLocation) const;

	/*Set element ID*/
	void SetElementId(const FVector Location, FOctreeElementId Id);

	/*Hold cover point*/
	bool OccupyCover(FOctreeElementId ElementID);

	// Mark the cover at the supplied location as taken.
	// Returns true if the cover wasn't already taken, false if it was or an error has occurred, e.g. the cover no longer exists.
	bool HoldCover(FOctreeElementId ElementID);
	bool HoldCover(FCoverPointOctreeElement Element);
	
	// Releases a cover that was taken.
	// Returns true if the cover was taken before, false if it wasn't or an error has occurred, e.g. the cover no longer exists.
	bool ReleaseCover(FOctreeElementId ElementID);
	bool ReleaseCover(FCoverPointOctreeElement Element);

	/*Add location to our TMap*/
	void SetElementIdImpl(const FVector Location, FOctreeElementId Id);
};	