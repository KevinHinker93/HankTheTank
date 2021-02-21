// Copyright Epic Games, Inc. All Rights Reserved.

#include "HankTheTankGameMode.h"
#include "Characters/HankTheTankPawn.h"

AHankTheTankGameMode::AHankTheTankGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AHankTheTankPawn::StaticClass();
}

