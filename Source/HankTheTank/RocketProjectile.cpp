// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Components/SphereComponent.h"
#include "Utility/LogCategoryDefinitions.h"
#include "GameFramework/ProjectileMovementComponent.h"

ARocketProjectile::ARocketProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	HomingDetectionTriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("HomingTrigger"));
	HomingDetectionTriggerComponent->SetupAttachment(RootComponent);
}

void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();

	HomingDetectionTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ARocketProjectile::OnDetectionTriggerOverlap);
	SetProjectileVelocity(fStartingVelocity);
}

void ARocketProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float fCurrentProjectileForwardVelocity = ProjectileMovement->Velocity.Size();
	if (fCurrentProjectileForwardVelocity < fMaxVelocity)
	{
		if (fTimeToReachMaxVelocityInSeconds > 0.0f)
		{
			float fVelocityGainPerSecond = ((fMaxVelocity - fStartingVelocity) / fTimeToReachMaxVelocityInSeconds) * DeltaSeconds;
			float fNewProjectileVelocity = FMath::Min(fCurrentProjectileForwardVelocity + fVelocityGainPerSecond, fMaxVelocity);
			SetProjectileVelocity(fNewProjectileVelocity);
		}
		else
		{
			UE_LOG(LogPlayerTank, Log, TEXT("Cannot adjust velocity of %s, becuase fTimeToReachMaxVelocityInSeconds is 0.0f"), *GetName());
		}
	}
}

void ARocketProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddRadialImpulse(Hit.ImpactPoint, fExplosionRadius, GetVelocity().Size() * fBaseImpulseImpactStrength, ERadialImpulseFalloff::RIF_Linear);
	}

	Explode(true);
}
// TODO: assign to on death of homing actor so it can acquire a new target, and find closest homing target
void ARocketProjectile::OnDetectionTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnStartHoming(OtherActor);
}

void ARocketProjectile::OnStartHoming(const AActor* HomingTarget)
{
	HomingDetectionTriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
}

void ARocketProjectile::SetProjectileVelocity(const float fVelocity)
{
	ProjectileMovement->Velocity += (GetActorForwardVector() * (fVelocity - ProjectileMovement->Velocity.Size()));
	ProjectileMovement->MaxSpeed = fVelocity;
}
