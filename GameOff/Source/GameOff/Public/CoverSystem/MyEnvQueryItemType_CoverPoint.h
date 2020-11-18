// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "CoverSystem/CoverPoint.h"
#include "MyEnvQueryItemType_CoverPoint.generated.h"

struct FBlackboardKeySelector;
class UBlackboardComponent; 
/**
 * 
 */
UCLASS()
class GAMEOFF_API UMyEnvQueryItemType_CoverPoint : public UEnvQueryItemType_VectorBase
{
	GENERATED_UCLASS_BODY()
	
public:

	typedef const FCoverPoint FValueType;

	static FCoverPoint GetValue(const uint8* RawData);
	static void SetValue(uint8* RawData, const FCoverPoint& Value);
	
	virtual FVector GetItemLocation(const uint8* RawData) const override;

	//virtual void AddBlackboardFilters(FBlackboardKeySelector& KeySelector, UObject* FilterOwner) const override;
	virtual bool StoreInBlackboard(FBlackboardKeySelector& KeySelector, UBlackboardComponent* Blackboard, const uint8* RawData) const override;
};
