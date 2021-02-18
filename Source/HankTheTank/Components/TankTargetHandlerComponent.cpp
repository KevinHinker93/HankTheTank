// Fill out your copyright notice in the Description page of Project Settings.


#include "TankTargetHandlerComponent.h"
#include "../Utility/LogCategoryDefinitions.h"
#include "../Utility/StaticHelperFunctions.h"
#include "DrawDebugHelpers.h"

UTankTargetHandlerComponent::UTankTargetHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

AActor* UTankTargetHandlerComponent::GetTargetActor() const
{
	return TargetActor;
}

FVector UTankTargetHandlerComponent::GetTargetLocation() const
{
	return TargetLocation;
}

void UTankTargetHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetTankController();
}

void UTankTargetHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TrackTarget();
}

void UTankTargetHandlerComponent::SetTankController()
{
	APawn* TankPawn = Cast<APawn>(GetOwner());
	if (TankPawn)
	{
		AController* TankController = nullptr;
		GET_OBJ_CHECKED(TankController, TankPawn->GetController(), LogPlayerTank, TEXT("Tank: %s found no controller!"), *TankPawn->GetName());
		TankPlayerController = Cast<APlayerController>(TankController);
	}
}

void UTankTargetHandlerComponent::TrackTarget()
{
	if (GetOwner())
	{
		FVector MousePosition, MouseDirection;
		EXECUTE_FUNC_CHECKED(TankPlayerController, TankPlayerController->DeprojectMousePositionToWorld(MousePosition, MouseDirection), LogPlayerTank,
			TEXT("Player pawn tank: %s has no PlayerController to retrieve the mouse position in world coordinates!"), *GetOwner()->GetName());

		// Adjust mouse world position, so we still get correct results if the camera is tilted
		// Mouse position will be projected onto the tank plane, so the actual mouse position can be correctly placed in the world
		// Otherwise it would be directly in front of the camera
		FVector AdjustedMousePosition = FMath::LinePlaneIntersection(
			MousePosition,
			MousePosition + (MouseDirection * 50000.f),
			GetOwner()->GetActorLocation(),
			FVector{ 0.f, 0.f, 1.f }
		);

		TargetLocation = AdjustedMousePosition;

		if (bDebugDrawMousePosition && GetWorld())
		{
			FVector DrawPos = TargetLocation;
			DrawPos.Z = 200.0f;
			DrawDebugPoint(GetWorld(), DrawPos, 4.85f, FColor(0, 0, 180), false, 0.1f, 0);
		}
	}
}

