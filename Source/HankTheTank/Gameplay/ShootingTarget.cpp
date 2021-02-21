// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingTarget.h"
#include "Components/StaticMeshComponent.h"

DEFINE_LOG_CATEGORY(LogTargets);

AShootingTarget::AShootingTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TargetRoot"));

	TargetActorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetActorMesh"));
	TargetActorMeshComponent->SetupAttachment(RootComponent);
	TargetActorMeshComponent->BodyInstance.SetCollisionProfileName("Enemy");
}

void AShootingTarget::Die()
{
	UE_LOG(LogTargets, Log, TEXT("Target %s will be destroyed"), *GetName());
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

