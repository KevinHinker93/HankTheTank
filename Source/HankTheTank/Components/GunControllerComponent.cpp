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
	RotateTowardsTarget();
}

void UGunControllerComponent::RotateTowardsTarget()
{
	EXECUTE_BLOCK_CHECKED(TargetHandlerComponent, LogPlayerTank,
		TEXT("Tank: %s could not rotate towards target, because the TankTargetHandlerComponent is null"), *GetOwner()->GetName())
	{
		FVector TargetLocation = TargetHandlerComponent->GetTargetLocation();
		TargetLocation.Z = 0.0f;


		FRotator LookAtTargetRotation = UKismetMathLibrary::FindLookAtRotation(GunTowerComponentToControl->GetComponentLocation(), TargetLocation);
		FVector GunTowerRotationInEulerAngles = GunTowerComponentToControl->GetComponentRotation().Euler();
		FRotator TargetRotation = FRotator::MakeFromEuler(FVector(GunTowerRotationInEulerAngles.X, GunTowerRotationInEulerAngles.Y, LookAtTargetRotation.Euler().Z));
		GunTowerComponentToControl->SetRelativeRotation(TargetRotation);


		FVector GunTowerToMouseDirection = TargetLocation - GunTowerComponentToControl->GetComponentLocation();
		GunTowerToMouseDirection.Normalize();
		FVector GunTowerFacingDirection = GunTowerComponentToControl->GetForwardVector();
		GunTowerFacingDirection.Z = 0.0f;

		UE_LOG(LogPlayerTank, Log, TEXT("Mouse loc: %s"), *TargetLocation.ToString());

		FVector DebugTargetEndPos = TargetLocation;
		DebugTargetEndPos.Z = GunTowerComponentToControl->GetComponentLocation().Z;
		DrawDebugLine(GetWorld(), GunTowerComponentToControl->GetComponentLocation(), DebugTargetEndPos, FColor(180.0f, 0.0f, 0.0f), false, -1, 0, 7.1f);

		/*float zRot = FMath::RadiansToDegrees(
			FVector::DotProduct(GunTowerFacingDirection, GunTowerToMouseDirection) / (GunTowerFacingDirection.Size2D() * GunTowerToMouseDirection.Size2D()));
		UE_LOG(LogPlayerTank, Log, TEXT("z rot: %f"), zRot);
		FVector GunTowerRotationInEulerAngles = GunTowerComponentToControl->GetComponentRotation().Euler();
		FRotator TargetRotation = FRotator::MakeFromEuler(FVector(GunTowerRotationInEulerAngles.X, GunTowerRotationInEulerAngles.Y, zRot));
		GunTowerComponentToControl->SetWorldRotation(TargetRotation);*/
	}
}

