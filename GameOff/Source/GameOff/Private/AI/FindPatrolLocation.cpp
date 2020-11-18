// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/FindPatrolLocation.h"
#include "AI/BotTargetPoint.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "NavigationSystem.h"


EBTNodeResult::Type UFindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (Controller)
	{
		AActor* CurrentWaypoint = Controller->GetCurrentWaypoint();
		if (CurrentWaypoint)
		{
			const float SearchRadius = 200.0f;
			const FVector SearchOrigin = CurrentWaypoint->GetActorLocation();

			FNavLocation Location;
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(Controller);
			if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, Location))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsVector("LocationToGo", Location.Location);
				return EBTNodeResult::Succeeded;
			} 
			else 
			{
				GLog->Log("NavSystem failed");
			}
		}
		else 
		{
			GLog->Log("Waypoint error");
		}
	}

	return EBTNodeResult::Failed;
}