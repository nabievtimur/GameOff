// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverSystem/EQS_GetGeneratedCovers.h"
#include "CoverSystem/MyEnvQueryItemType_CoverPoint.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "CoverSystem/CoverGenerator.h"
#include "Containers/Array.h"

#define LOCTEXT_NAMESPACE "UEQS_GetGeneratedCovers"

UEQS_GetGeneratedCovers::UEQS_GetGeneratedCovers()
{
	GenerateAround = UEnvQueryContext_Querier::StaticClass();
	ItemType = UMyEnvQueryItemType_CoverPoint::StaticClass();
	SquareExtent.DefaultValue = 750.f;
	BoxHeight.DefaultValue = 200.f;
}

void UEQS_GetGeneratedCovers::GenerateItems(FEnvQueryInstance& QueryInstance) const 
{
	UObject* BindOwner = QueryInstance.Owner.Get();

	SquareExtent.BindData(BindOwner, QueryInstance.QueryID);
	BoxHeight.BindData(BindOwner, QueryInstance.QueryID);

	const float SquareE = SquareExtent.GetValue();	
	const float BoxH = BoxHeight.GetValue();

	TArray<FVector> ContextLocations;
	QueryInstance.PrepareContext(GenerateAround, ContextLocations);

	ACoverGenerator* CoverGenerator = ACoverGenerator::GetCoverGenerator(QueryInstance.World);

	if (!CoverGenerator)
	{
		GLog->Log(TEXT("Failed to retrieve CoverGenerator."));
		return;
	}
	
	TArray<FCoverPoint> Covers;
	for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ContextIndex++)
	{
		CoverGenerator->GetCoverWithinBounds(FBoxCenterAndExtent(ContextLocations[ContextIndex], FVector(SquareE, SquareE, BoxH)),
											Covers);
		//GetFreeLocationsFromCovers(Covers, NavPoints);
	}
	QueryInstance.AddItemData<UMyEnvQueryItemType_CoverPoint>(Covers);
	//StoreNavPoints(NavPoints, QueryInstance);
}

// void UEQS_GetGeneratedCovers::GetFreeLocationsFromCovers(const TArray<FCoverPoint>& CoverPoints, TArray<FNavLocation>& NavPoints) const
// {
// 	for (FCoverPoint Iter : CoverPoints)
// 	{
// 		if (!Iter->Occupied)
// 		{
// 			NavPoints.Add(FNavLocation(Iter.Location));
// 		}
// 	}
// }

FText UEQS_GetGeneratedCovers::GetDescriptionTitle() const
{
	return FText::Format(LOCTEXT("DescriptionGenerateAroundContext", "{0}: generate around {1}"),
		Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(GenerateAround));
};

FText UEQS_GetGeneratedCovers::GetDescriptionDetails() const
{
	FText Desc = FText::Format(LOCTEXT("SimpleGridDescription", "Extent: {0}, {0}, {1}"),
		FText::FromString(SquareExtent.ToString()),
		FText::FromString(BoxHeight.ToString()));
	return Desc;
}

#undef LOCTEXT_NAMESPACE
