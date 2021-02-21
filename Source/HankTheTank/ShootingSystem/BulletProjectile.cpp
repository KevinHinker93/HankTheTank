// Copyright Epic Games, Inc. All Rights Reserve

#include "BulletProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Utility/StaticHelperFunctions.h"
#include "../Utility/LogCategoryDefinitions.h"

ABulletProjectile::ABulletProjectile()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");

	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ABulletProjectile::Explode(bool bShouldDestroyParticle)
{
	EXECUTE_BLOCK_CHECKED(GetWorld(), LogPlayerTank, TEXT("Projectile %s could not explode, because world was null"), *GetName())
	{
		FVector ExplosionLocation = GetActorLocation();
		FRotator ExplosionRotation = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, ExplosionLocation, ExplosionRotation);

		if (bShouldDestroyParticle)
		{
			Destroy();
		}
	}
}

void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();

	EXECUTE_BLOCK_CHECKED(ProjectileMesh, LogPlayerTank, TEXT("Projectile %s could not bind to collision events, because collider component was null"), *GetName())
	{
		ProjectileMesh->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnHit);
		ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ABulletProjectile::OnOverlap);
	}
}

void ABulletProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABulletProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * fBaseImpulseImpactStrength, GetActorLocation());
	}

	Explode(true);
}

void ABulletProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->CanBeDamaged())
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, 1.0f, SweepResult.ImpactNormal, SweepResult, nullptr, this, ProjectileDamageTypeClass);
	}

	Explode(true);
}

void ABulletProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnProjectileHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void ABulletProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnProjectileOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
