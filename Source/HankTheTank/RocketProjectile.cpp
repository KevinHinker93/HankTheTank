// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ARocketProjectile::ARocketProjectile()
{
	HomingDetectionTriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("HomingTrigger"));
	HomingDetectionTriggerComponent->SetupAttachment(RootComponent);
}

void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();

	HomingDetectionTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ARocketProjectile::OnDetectionTriggerOverlap);
}

void ARocketProjectile::OnDetectionTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnStartHoming(OtherActor);
}

void ARocketProjectile::OnStartHoming(const AActor* HomingTarget)
{
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
}
