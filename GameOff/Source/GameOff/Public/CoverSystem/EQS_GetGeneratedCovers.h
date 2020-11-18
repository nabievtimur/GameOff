// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "DataProviders/AIDataProvider.h"
#include "CoverSystem/CoverPoint.h"
#include "EQS_GetGeneratedCovers.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF_API UEQS_GetGeneratedCovers : public UEnvQueryGenerator
{
	GENERATED_BODY()
	
	UEQS_GetGeneratedCovers();

	/** Square's extent */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue SquareExtent;

	/** Box's height */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue BoxHeight;

	/** context */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	TSubclassOf<UEnvQueryContext> GenerateAround;

	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	void GetFreeLocationsFromCovers(const TArray<FCoverPoint*>& CoverPoints, TArray<FNavLocation>& NavPoints) const;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
