// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/ContextPlayer.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UContextPlayer::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
 
	/* Get the player actor*/
	AActor* PlayerActor = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	/* Set the context Player on provided context data */
	if (PlayerActor) 
	{ 
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerActor);
	}
}




