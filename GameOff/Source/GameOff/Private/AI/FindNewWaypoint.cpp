// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FindNewWaypoint.h"
#include "AI/BotTargetPoint.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


EBTNodeResult::Type UFindNewWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* BotController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (BotController)
	{
		TArray<AActor*> Waypoints;
		UGameplayStatics::GetAllActorsOfClass(BotController, ABotTargetPoint::StaticClass(), Waypoints);

		if (Waypoints.Num() == 0) 
		{
			return EBTNodeResult::Failed;
		}

		AActor* NewWaypoint = Waypoints[FMath::RandRange(0, Waypoints.Num() - 1)];

		if (NewWaypoint) 
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentWaypoint", NewWaypoint);
			return EBTNodeResult::Succeeded;		
		}
	}
	return EBTNodeResult::Failed;
}

