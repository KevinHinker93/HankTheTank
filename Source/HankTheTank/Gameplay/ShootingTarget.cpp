// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingTarget.h"
#include "Components/StaticMeshComponent.h"

AShootingTarget::AShootingTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	TargetActorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetActorMesh"));
	RootComponent = TargetActorMeshComponent;
	TargetActorMeshComponent->BodyInstance.SetCollisionProfileName("Enemy");
}

void AShootingTarget::Die()
{
	Destroy();
}

void AShootingTarget::BeginPlay()
{
	Super::BeginPlay();
}

void AShootingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AShootingTarget::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float ReceivedDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Die();

	return ReceivedDamage;
}

