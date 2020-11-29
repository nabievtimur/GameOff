// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "IsPointFreeTest.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF_API UIsPointFreeTest : public UEnvQueryTest
{
	GENERATED_BODY()
		
	UIsPointFreeTest();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;	
};
