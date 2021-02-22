// Fill out your copyright notice in the Description page of Project Settings.


#include "GunControllerComponent.h"
#include "../Utility/LogCategoryDefinitions.h"
#include "../Utility/StaticHelperFunctions.h"
#include "TankTargetHandlerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UGunControllerComponent::UGunControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGunControllerComponent::PreserveOriginalTowerRotation(const float fPreviousTankZTowerRot)
{
	AActor* OwningActor = GetOwner();
	if (OwningActor)
	{
		EXECUTE_BLOCK_CHECKED(GunTowerComponentToControl, LogPlayerTank, TEXT("%s could not preserve rotation, because GunTowerComponentToControl is nullptr"), *OwningActor->GetName())
		{
			FVector TowerRot = GunTowerComponentToControl->GetComponentRotation().Euler();
			TowerRot.Z = fPreviousTankZTowerRot;
			GunTowerComponentToControl->SetWorldRotation(FRotator::MakeFromEuler(TowerRot));
		}
	}
}

void UGunControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get and set neccessary components
	AActor* OwningActor = GetOwner();
	if (OwningActor)
	{
		GET_OBJ_CHECKED(TargetHandlerComponent, UStaticHelperFunctions::GetActorCompAs<UTankTargetHandlerComponent>(OwningActor), LogPlayerTank,
			TEXT("Tank: %s has no TankTargetHandlerComponent assigned, have you forgot to add one?"), *OwningActor->GetName());

		GET_OBJ_CHECKED(GunTowerComponentToControl, UStaticHelperFunctions::GetActorCompByTagAs<USceneComponent>(OwningActor, nGunTowerComponentTagName), LogPlayerTank,
			TEXT("Tank: %s could not find a SceneComponent with tag: %s, have you forgot to add one or set the tag?"),
			*OwningActor->GetName(), *nGunTowerComponentTagName.ToString());
	}
}

void UGunControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetCurrentAngleBetweenGunTowerAndTarget();

	// Only allow rotation when the mouse pointer at least moved fGunAngularThreshold angles
	if (!bRotationIsDirty && FMath::Abs(fCurrentDesiredZTowerRotation) >= fGunAngularThreshold)
	{
		bRotationIsDirty = true;
	}

	if (bRotationIsDirty)
	{
		RotateTowardsTarget();
	}
}

void UGunControllerComponent::SetCurrentAngleBetweenGunTowerAndTarget()
{
	EXECUTE_BLOCK_CHECKED(TargetHandlerComponent, LogPlayerTank,
		TEXT("Tank: %s could not calculate tower to mouse angle, because the TankTargetHandlerComponent is null"), *GetOwner()->GetName())
	{
		// Get target position, since the mouse pointer will not yield an actor as target
		FVector TargetLocation = TargetHandlerComponent->GetTargetLocation();
		TargetLocation.Z = 0.0f; // z component not neccessary for calculations

		EXECUTE_BLOCK_CHECKED(GunTowerComponentToControl, LogPlayerTank,
			TEXT("Tank: %s could not calculate tower to mouse angle, because the GunTowerComponentToControl is null"), *GetOwner()->GetName())
		{

			FRotator LookAtTargetRotation = UKismetMathLibrary::FindLookAtRotation(GunTowerComponentToControl->GetComponentLocation(), TargetLocation);
			fCurrentDesiredZTowerRotation = LookAtTargetRotation.Euler().Z; // Only z component is relevant for tower rotation

			FVector GunTowerLocation = GunTowerComponentToControl->GetComponentLocation();
			GunTowerLocation.Z = 0.0f;
			FVector GunTowerToMouseDirection = TargetLocation - GunTowerLocation;
			GunTowerToMouseDirection.Normalize();
			FVector GunTowerRightDirection = GunTowerComponentToControl->GetRightVector();
			GunTowerRightDirection.Z = 0.0f;

			if (FVector::DotProduct(GunTowerToMouseDirection, GunTowerRightDirection) < 0)
			{
				fSign = -1; // Mouse pointer is left of gun
			}
			else
			{
				fSign = 1.0f; // Mouse pointer is right of gun
			}

			if (bDebugDrawLineFromTowerToMouse && GetWorld())
			{
				FVector DebugTargetEndPos = TargetLocation;
				DebugTargetEndPos.Z = GunTowerComponentToControl->GetComponentLocation().Z;
				DrawDebugLine(GetWorld(), GunTowerComponentToControl->GetComponentLocation(), DebugTargetEndPos, FColor(180.0f, 0.0f, 0.0f), false, -1, 0, 7.1f);
			}
		}
	}
}

void UGunControllerComponent::RotateTowardsTarget()
{
	EXECUTE_BLOCK_CHECKED(GunTowerComponentToControl, LogPlayerTank,
		TEXT("Tank: %s could not rotate towards target, because the GunTowerComponentToControl is null"), *GetOwner()->GetName())
	{
		FRotator GunTowerRotation = GunTowerComponentToControl->GetComponentRotation();
		float fGunTowerZRotationInDegrees = GunTowerRotation.Euler().Z; // Only z component is relevant for tower rotation

		if (GetWorld())
		{
			// Create rotation vector, fSign determines the direction of the rotation
			FVector AngularRotation = GunTowerRotation.RotateVector(FVector(0, 0, fSign * fGunAngularVelocity * GetWorld()->DeltaTimeSeconds));
			GunTowerComponentToControl->AddWorldRotation(FRotator::MakeFromEuler(AngularRotation), false);

			if (FMath::Abs(fGunTowerZRotationInDegrees - fCurrentDesiredZTowerRotation) < fAngularTolerance)
			{
				bRotationIsDirty = false;
				// Set tank tower rotation to desired rotation to avoid rotation offset
				FVector GunTowerRotationInEulerAngles = GunTowerComponentToControl->GetComponentRotation().Euler();
				FRotator TargetRotation = FRotator::MakeFromEuler(FVector(GunTowerRotationInEulerAngles.X, GunTowerRotationInEulerAngles.Y, fCurrentDesiredZTowerRotation));
				GunTowerComponentToControl->SetWorldRotation(TargetRotation);
			}
		}
	}
}

