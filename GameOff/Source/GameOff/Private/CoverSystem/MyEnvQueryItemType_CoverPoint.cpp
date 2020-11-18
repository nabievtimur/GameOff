// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverSystem/MyEnvQueryItemType_CoverPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"


UMyEnvQueryItemType_CoverPoint::UMyEnvQueryItemType_CoverPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ValueSize = sizeof(FCoverPoint);
}

FCoverPoint UMyEnvQueryItemType_CoverPoint::GetValue(const uint8* RawData)
{
	return GetValueFromMemory<FCoverPoint>(RawData);
}

void UMyEnvQueryItemType_CoverPoint::SetValue(uint8* RawData, const FCoverPoint& Value)
{
	SetValueInMemory<FCoverPoint>(RawData, Value);
}

FVector UMyEnvQueryItemType_CoverPoint::GetItemLocation(const uint8 * RawData) const
{
	FCoverPoint MyCover = UMyEnvQueryItemType_CoverPoint::GetValue(RawData);
	return MyCover.Location;
}

//TODO
// void UMyEnvQueryItemType_CoverPoint::AddBlackboardFilters(FBlackboardKeySelector& KeySelector, UObject* FilterOwner) const
// {
// 	Super::AddBlackboardFilters(KeySelector, FilterOwner);
// 	KeySelector.AddObjectFilter(FilterOwner, GetClass()->GetFName(), AActor::StaticClass());
// }

bool UMyEnvQueryItemType_CoverPoint::StoreInBlackboard(FBlackboardKeySelector& KeySelector, UBlackboardComponent* Blackboard, const uint8* RawData) const
{
	bool bStored = Super::StoreInBlackboard(KeySelector, Blackboard, RawData);
	if (!bStored && KeySelector.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UCoverPointObj* CoverPointObj = NewObject<UCoverPointObj>();  
		FCoverPoint CoverPoint = GetValue(RawData);
		CoverPointObj->InitCoverPoint(CoverPoint);
		Blackboard->SetValue<UBlackboardKeyType_Object>(KeySelector.GetSelectedKeyID(), Cast<UObject>(CoverPointObj));

		bStored = true;
	}

 	return bStored;
}
