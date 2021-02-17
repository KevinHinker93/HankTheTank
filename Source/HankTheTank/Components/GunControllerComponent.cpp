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

void UGunControllerComponent::BeginPlay()
{
	Super::BeginPlay();

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
		FVector TargetLocation = TargetHandlerComponent->GetTargetLocation();
		TargetLocation.Z = 0.0f;


		FRotator LookAtTargetRotation = UKismetMathLibrary::FindLookAtRotation(GunTowerComponentToControl->GetComponentLocation(), TargetLocation);
		fCurrentDesiredZTowerRotation = LookAtTargetRotation.Euler().Z;

		if (bDebugDrawLineFromTowerToMouse)
		{
			FVector GunTowerLocation = GunTowerComponentToControl->GetComponentLocation();
			GunTowerLocation.Z = 0.0f;
			FVector GunTowerToMouseDirection = TargetLocation - GunTowerLocation;
			GunTowerToMouseDirection.Normalize();
			FVector GunTowerFacingDirection = GunTowerComponentToControl->GetForwardVector();
			GunTowerFacingDirection.Z = 0.0f;

			UE_LOG(LogPlayerTank, Log, TEXT("Mouse loc: %s"), *TargetLocation.ToString());

			FVector DebugTargetEndPos = TargetLocation;
			DebugTargetEndPos.Z = GunTowerComponentToControl->GetComponentLocation().Z;
			DrawDebugLine(GetWorld(), GunTowerComponentToControl->GetComponentLocation(), DebugTargetEndPos, FColor(180.0f, 0.0f, 0.0f), false, -1, 0, 7.1f);

		}
	}
}

void UGunControllerComponent::RotateTowardsTarget()
{
	EXECUTE_BLOCK_CHECKED(TargetHandlerComponent, LogPlayerTank,
		TEXT("Tank: %s could not rotate towards target, because the TankTargetHandlerComponent is null"), *GetOwner()->GetName())
	{
		FRotator GunTowerRotation = GunTowerComponentToControl->GetComponentRotation();
		float fGunTowerZRotationInDegrees = GunTowerRotation.Euler().Z;

		if (FMath::Abs(fGunTowerZRotationInDegrees - fCurrentDesiredZTowerRotation) < fGunAngularThreshold)
		{
			bRotationIsDirty = false;
			FVector GunTowerRotationInEulerAngles = GunTowerComponentToControl->GetComponentRotation().Euler();
			FRotator TargetRotation = FRotator::MakeFromEuler(FVector(GunTowerRotationInEulerAngles.X, GunTowerRotationInEulerAngles.Y, fGunTowerZRotationInDegrees));
		}
		else
		{
			float sign = -FMath::Sign(FMath::FindDeltaAngleRadians(fCurrentDesiredZTowerRotation, fGunTowerZRotationInDegrees));
			FVector AngularRotation = GunTowerRotation.RotateVector(FVector(0, 0, sign * fGunAngularVelocity * GetWorld()->DeltaTimeSeconds));
			GunTowerComponentToControl->AddWorldRotation(FRotator::MakeFromEuler(AngularRotation));
		}
	}
}

