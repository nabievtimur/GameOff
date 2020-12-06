// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
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
 
    Sight->SightRadius = 2000.0f;
    Sight->LoseSightRadius = 2000.0f;
    Sight->PeripheralVisionAngleDegrees = 135.0f;
 
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

    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    bIsHiding = BlackboardComp->GetValueAsBool(HidingKey);
    if (!bCanSeePlayer && !bIsHiding && !LostPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("LostTimer inc"));
        PlayerLostTimer++;
    }

    if (PlayerLostTimer == LostTime)
    {
        BlackboardComp->SetValueAsObject(TargetKey, NULL);
        PlayerLostTimer = 0;
        LostPlayer = true;
    }
}

void AEnemyAIController::OnPossess(APawn* InPawn) 
{
	Super::OnPossess(InPawn);

	/*Get the possessed Pawn. If it's the AI Character we created
    initialize it's blackboard and start it's corresponding behavior tree*/

	ADefaultEnemy* AICharacter = Cast<ADefaultEnemy>(InPawn);
    this->EnemyPawn = AICharacter;

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
    
void AEnemyAIController::OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus)
{
    AActor* PlayerActor = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (SensedActor == PlayerActor)
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            UE_LOG(LogTemp, Warning, TEXT("Wants fire"));
            bCanSeePlayer = true;
            BlackboardComp->SetValueAsBool(CanSeePlayerKey, true);
            BlackboardComp->SetValueAsObject(TargetKey, SensedActor);
            BlackboardComp->SetValueAsBool(WantsFire, true);
            if (BlackboardComp->GetValueAsBool(HidingKey))
            {
                GLog->Log("Set player while hiding");
                BlackboardComp->SetValueAsBool(InCoverKey, false);
                BlackboardComp->SetValueAsBool(HidingKey, false);
            }
            LostPlayer = false;
        } 
        else
        {
            BlackboardComp->SetValueAsBool(WantsFire, false);
            UE_LOG(LogTemp, Warning, TEXT("Not wants fire"));
            bCanSeePlayer = false;
            BlackboardComp->SetValueAsBool(CanSeePlayerKey, false);
            BlackboardComp->SetValueAsVector(LastSeenPlayerKey, SensedActor->GetActorLocation());
            PlayerLostTimer = 0;
        }
    }
}

AActor* AEnemyAIController::GetSeeingPawn()
{
    //Return the seeing pawn
    UObject* object = BlackboardComp->GetValueAsObject(TargetKey);
 
    return object ?  Cast<AActor>(object) : nullptr;
}
