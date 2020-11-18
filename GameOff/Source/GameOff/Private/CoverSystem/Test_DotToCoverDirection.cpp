// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverSystem/Test_DotToCoverDirection.h"
#include "CoverSystem/CoverPoint.h"
#include "CoverSystem/MyEnvQueryItemType_CoverPoint.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "CoverSystem/CoverGenerator.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

UTest_DotToCoverDirection::UTest_DotToCoverDirection()
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UMyEnvQueryItemType_CoverPoint::StaticClass();
	SetWorkOnFloatValues(false);

	Context = UEnvQueryContext_Querier::StaticClass();
}

void UTest_DotToCoverDirection::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* DataOwner = QueryInstance.Owner.Get();
	MaxAngle.BindData(DataOwner, QueryInstance.QueryID);

	const float _MaxAngle = MaxAngle.GetValue();

	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(Context, ContextLocations))
	{
		return;
	}

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		FCoverPoint CoverPoint = UMyEnvQueryItemType_CoverPoint::GetValue(QueryInstance.RawData.GetData() + QueryInstance.Items[It.GetIndex()].DataOffset);

		const FVector CoverDirection = CoverPoint.PerpToWall;
		const FVector CoverPosition = CoverPoint.Location; 

		for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ContextIndex++)
		{
			const FVector ContextLocation = ContextLocations[ContextIndex]; 
			FVector DirectionContextToCover = ContextLocation - CoverPosition; 
			DirectionContextToCover.Normalize();

			float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CoverDirection, DirectionContextToCover)));
			//UE_LOG(LogTemp, Warning, TEXT("Angle is %f, coverdir %s"), Angle, *CoverDirection.ToString());
			// Check if parallel
			bool bParallel = Angle < _MaxAngle;
			It.SetScore(TestPurpose, FilterType, bParallel, true);
		}
	}		
}




