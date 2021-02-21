// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileProjectile.h"
#include "Components/SphereComponent.h"
#include "../Utility/LogCategoryDefinitions.h"
#include "../Utility/StaticHelperFunctions.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMissileProjectile::AMissileProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	HomingDetectionTriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("HomingTrigger"));
	HomingDetectionTriggerComponent->SetupAttachment(RootComponent);
}

void AMissileProjectile::BeginPlay()
{
	Super::BeginPlay();

	EXECUTE_FUNC_CHECKED(HomingDetectionTriggerComponent, HomingDetectionTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AMissileProjectile::OnDetectionTriggerOverlap),
		LogShooting, TEXT("Projectile %s could not bind to the detection radius overlap event, because it was null"), *GetName());

	SetProjectileVelocity(fStartingVelocity);
}

void AMissileProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	EXECUTE_BLOCK_CHECKED(ProjectileMovement, LogShooting, TEXT("Cannot adjust velocity of %s, becuase ProjectileMovement is null"), *GetName())
	{
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
				UE_LOG(LogShooting, Log, TEXT("Cannot adjust velocity of %s, becuase fTimeToReachMaxVelocityInSeconds is 0.0f"), *GetName());
			}
		}
	}
}

void AMissileProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddRadialImpulse(Hit.ImpactPoint, fExplosionRadius, GetVelocity().Size() * fBaseImpulseImpactStrength, ERadialImpulseFalloff::RIF_Linear);
	}

	Explode(true);
}

void AMissileProjectile::OnDetectionTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != CurrentHomingTargetActor && !IsTargetBlockedByAnObstacle(OtherActor))
	{
		if (IsNearestTarget(OtherActor))
		{
			OnStartHoming(OtherActor);
		}
	}
}

void AMissileProjectile::OnStartHoming(AActor* HomingTarget)
{
	EXECUTE_BLOCK_CHECKED(ProjectileMovement, LogShooting, TEXT("Projectile %s cannot start homing, becuase ProjectileMovement is null"), *GetName())
	{
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		CurrentHomingTargetActor = HomingTarget;
		UE_LOG(LogShooting, Log, TEXT("Projectile %s starts homing %s"), *GetName(), *HomingTarget->GetName());
	}
}

void AMissileProjectile::SetProjectileVelocity(const float fVelocity)
{
	EXECUTE_BLOCK_CHECKED(ProjectileMovement, LogShooting, TEXT("Cannot set velocity of %s, becuase ProjectileMovement is null"), *GetName())
	{
		ProjectileMovement->Velocity += (GetActorForwardVector() * (fVelocity - ProjectileMovement->Velocity.Size()));
		ProjectileMovement->MaxSpeed = fVelocity;
	}
}

bool AMissileProjectile::IsNearestTarget(const AActor* Target)
{
	bool bIsNearest = false;
	if (CurrentHomingTargetActor)
	{
		FVector ProjectileLocation = GetActorLocation();
		FVector NewTargetLocation = Target->GetActorLocation();
		FVector CurrentHomingTargetLocation = CurrentHomingTargetActor->GetActorLocation();

		FVector ProjectileToNewTargetVector = NewTargetLocation - ProjectileLocation;
		FVector ProjectileToCurrentTargetVector = CurrentHomingTargetLocation - ProjectileLocation;

		if (ProjectileToNewTargetVector.SizeSquared2D() < ProjectileToCurrentTargetVector.SizeSquared2D())
		{
			bIsNearest = true;
		}
	}
	else
	{
		// If there is no current homing target the new target is definately the nearest target
		bIsNearest = true;
	}

	return bIsNearest;
}

bool AMissileProjectile::IsTargetBlockedByAnObstacle(const AActor* Target)
{
	UWorld* world = GetWorld();
	EXECUTE_BLOCK_CHECKED(world, LogShooting, TEXT("Projectile %s could not check if target is blocked by an obstacle, because world is null"), *GetName())
	{
		FHitResult HitResult;
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = Target->GetActorLocation();

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(Target);
		TraceParams.AddIgnoredActor(this);

		return GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_WorldStatic, TraceParams);
	}

	return false;
}
