// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameOffGameMode.h"
#include "GameOffCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGameOffGameMode::AGameOffGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
