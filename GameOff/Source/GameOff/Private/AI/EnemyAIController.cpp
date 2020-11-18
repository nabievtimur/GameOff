// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Enemies/DefaultEnemy.h"
#include "GameFramework/Pawn.h"


/*Set defaults*/

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
 
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(FName("PerceptionComp"));
 
    //Create a Sight Sense
    Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));
 
    Sight->SightRadius = 1000.f;
    Sight->LoseSightRadius = 1500.f;
    Sight->PeripheralVisionAngleDegrees = 130.f;
 
    //Tell the sight sense to detect everything
    Sight->DetectionByAffiliation.bDetectEnemies = true;
    Sight->DetectionByAffiliation.bDetectFriendlies = true;
    Sight->DetectionByAffiliation.bDetectNeutrals = true;
 
    //Register the sight sense to our Perception Component
    PerceptionComp->ConfigureSense(*Sight);
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
}

void AEnemyAIController::OnPossess(APawn* InPawn) 
{
	Super::OnPossess(InPawn);

	/*Get the possessed Pawn. If it's the AI Character we created
    initialize it's blackboard and start it's corresponding behavior tree*/

	ADefaultEnemy* AICharacter = Cast<ADefaultEnemy>(InPawn);

	if (AICharacter)
    {
        if (BehaviorTree->BlackboardAsset)
        {
            BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
            BehaviorComp->StartTree(*BehaviorTree);
        }
    }
}

void AEnemyAIController::SetSeenTarget(APawn* aPawn)
{
    /*Registers the Pawn that the AI has seen in the blackboard*/
    if (this->BlackboardComp)
    {
        BlackboardComp->SetValueAsObject(TargetKey, aPawn);
    }
}

AActor* AEnemyAIController::GetCurrentWaypoint() 
{
	if (this->BlackboardComp)
	{
		return Cast<AActor>(BlackboardComp->GetValueAsObject(CurrentWaypointKey));
	}
	return nullptr;
}
    
void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    /* If our character exists inside the UpdatedActors array, register him
    to our blackboard component */
    
    GLog->Log("Updating perception");
    for (AActor* Actor : UpdatedActors)
    {
        if (!GetSeeingPawn())
        {
            BlackboardComp->SetValueAsObject(TargetKey, Actor);
            GLog->Log("Set target");

            return;
        }
    }
 
    /*The character doesn't exist in our updated actors - so make sure
    to delete any previous reference of him from the blackboard */
    BlackboardComp->SetValueAsObject(TargetKey, nullptr);
}

AActor* AEnemyAIController::GetSeeingPawn()
{
    //Return the seeing pawn
    UObject* object = BlackboardComp->GetValueAsObject(TargetKey);
 
    return object ?  Cast<AActor>(object) : nullptr;
}