// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverSystem/IsPointFreeTest.h"
#include "CoverSystem/CoverPoint.h"
#include "CoverSystem/MyEnvQueryItemType_CoverPoint.h"
#include "CoverSystem/CoverGenerator.h"

UIsPointFreeTest::UIsPointFreeTest()
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UMyEnvQueryItemType_CoverPoint::StaticClass();
	SetWorkOnFloatValues(false);
}

void UIsPointFreeTest::RunTest(FEnvQueryInstance& QueryInstance) const
{
	ACoverGenerator* CoverGenerator = ACoverGenerator::GetCoverGenerator(QueryInstance.World);
	if (CoverGenerator)
	{
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			// Get current CoverPoint
			FCoverPoint CoverPoint = UMyEnvQueryItemType_CoverPoint::GetValue(QueryInstance.RawData.GetData() + QueryInstance.Items[It.GetIndex()].DataOffset);
			bool IsFree = !(CoverPoint.Occupied); 

			It.SetScore(TestPurpose, FilterType, IsFree, true);
		}
	}
}