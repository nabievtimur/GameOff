// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "CoverGenerator.h"
#include "AI/EnemyAIController.h"
#include "TakeCover.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF_API UTakeCover : public UBTTaskNode
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CurrentCoverKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector NewCoverKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector InCoverKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector NeedCoverKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
