// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Enemies/DefaultEnemy.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

private:

	/* Behavior Tree component reference */
	UBehaviorTreeComponent* BehaviorComp;

	/* Blackboard comp reference */
	UBlackboardComponent* BlackboardComp;

	/* A Sight Sense config for our AI */
	UAISenseConfig_Sight* Sight;

	/*Controlled pawn*/
	ADefaultEnemy* EnemyPawn;

	/* */
	int PlayerLostTimer = 0;

	bool bCanSeePlayer = false;

	bool bIsHiding = false;

	bool LostPlayer = false;

	/* The function that fires when the perception of our AI gets updated */
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus stimulus);

public:

	/*Constructor*/
	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*Called when controller posses bot*/
	virtual void OnPossess(class APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	//virtual void onUnPosses() override;

	/*Sets the sensed target in the blackboard*/
	void SetSeenTarget(APawn* Pawn);

	/*Returns current location to go*/
	AActor* GetCurrentWaypoint();

	/* Returns the seeing pawn. Returns null, if our AI has no target */
	AActor* GetSeeingPawn();
	
	UFUNCTION(BlueprintImplementableEvent,  Category = "Cover")
	void TakeCover();	

	/*Blackboard keys*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FName TargetKey = "Target";	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FName CurrentWaypointKey = "CurrentWaypoint";	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FName LocationToGoKey = "LocationToGo";	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FName LastSeenPlayerKey = "LastSeenPlayer";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FName CanSeePlayerKey = "CanSeePlayer";

	UPROPERTY(Category="Perception", EditAnywhere)
	float SightRadius = 3000.0f;

	UPROPERTY(Category="Perception", EditAnywhere)
	float LoseRadius = 5000.0f;

	UPROPERTY(Category="Perception", EditAnywhere)
	float VisionAngleDegrees = 130.0f;

	UPROPERTY(Category="AI", EditAnywhere)
	int LostTime = 20;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	FName HidingKey = "Hiding";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	FName InCoverKey = "InCover";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover")
	FName WantsFire = "WantFire";

	/* The Component which is used for the "seeing" player */
	UPROPERTY(VisibleAnywhere, Category="AI")
    class UAIPerceptionComponent* PerceptionComp;

	/* The Behavior Tree of the Character */
    UPROPERTY(EditAnywhere,Category="AI")
    class UBehaviorTree* BehaviorTree;
};
