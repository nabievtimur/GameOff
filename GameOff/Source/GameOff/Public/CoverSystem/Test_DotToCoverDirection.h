// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "Test_DotToCoverDirection.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF_API UTest_DotToCoverDirection :  public UEnvQueryTest
{
	GENERATED_BODY()
	
	UTest_DotToCoverDirection();

	/*Max angle between vectors*/
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	FAIDataProviderFloatValue MaxAngle;

	/** context: other end of trace test */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	TSubclassOf<UEnvQueryContext> Context;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;	
};
