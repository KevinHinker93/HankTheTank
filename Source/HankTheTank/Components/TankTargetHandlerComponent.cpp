// Fill out your copyright notice in the Description page of Project Settings.


#include "TankTargetHandlerComponent.h"

UTankTargetHandlerComponent::UTankTargetHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UTankTargetHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* TankPawn = Cast<APawn>(GetOwner());
	AController* TankController = TankPawn->GetController();
	TankPlayerController = Cast<APlayerController>(TankController);
}

void UTankTargetHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTankTargetHandlerComponent::TrackTarget()
{
	FVector MouseLocation, MouseDirection;
	TankPlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);


}

