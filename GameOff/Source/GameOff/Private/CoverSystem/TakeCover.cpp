// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/CoverSystem/TakeCover.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


EBTNodeResult::Type UTakeCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* BotController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (BotController)
	{
		//FCoverPoint* CoverPoint = Cast<FCoverPoint>(BotController->GetCurrentWaypoint());
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

		FVector CurrentCoverLoc = Blackboard->GetValueAsVector(CurrentCoverKey.SelectedKeyName);
		FVector NewCoverLoc = Blackboard->GetValueAsVector(NewCoverKey.SelectedKeyName);

		ACoverGenerator* CoverGenerator = ACoverGenerator::GetCoverGenerator(GetWorld());

		//UCoverPointObj* CurrentCoverObj = Cast<UCoverPointObj>(Blackboard -> GetValueAsObject(CurrentCoverKey.SelectedKeyName));
		// CurrentCoverPoint->Location = CurrentCoverLoc;

		FCoverPoint CurrentCoverPoint;
		CurrentCoverPoint.Init(CurrentCoverLoc);
		CoverGenerator->ReleaseCover(CurrentCoverPoint);

		//UCoverPointObj* NewCoverObj =  Cast<UCoverPointObj>(Blackboard->GetValueAsObject(NewCoverKey.SelectedKeyName));;

		FCoverPoint NewCoverPoint;
		NewCoverPoint.Init(NewCoverLoc);
		CoverGenerator->OccupyCover(NewCoverPoint);

		Blackboard->SetValueAsVector(CurrentCoverKey.SelectedKeyName, NewCoverLoc);
		Blackboard->SetValueAsBool(NeedCoverKey.SelectedKeyName, false);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

