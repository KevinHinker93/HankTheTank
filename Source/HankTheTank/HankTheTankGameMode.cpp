// Copyright Epic Games, Inc. All Rights Reserved.

#include "HankTheTankGameMode.h"
#include "HankTheTankPawn.h"

AHankTheTankGameMode::AHankTheTankGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AHankTheTankPawn::StaticClass();
}

