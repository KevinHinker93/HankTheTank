// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletProjectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;
class UParticleSystem;

UCLASS(config=Game)
class ABulletProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABulletProjectile();

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
		float fBaseImpulseImpactStrength = 25.0f;

	/**
	* Called to let the projectile explode.
	*/
	virtual void Explode(bool bShouldDestroyParticle);

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UDamageType> ProjectileDamageTypeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* ExplosionParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovement;

	// ---------------------------------------------------------
	// Projectile collision functionality functions, used in child classes to achieve custom behavior

	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// ---------------------------------------------------------

private:

	// ---------------------------------------------------------
	// Projectile collision callbacks, these are bind automatically at begin play
	// Use OnProjectileHit and/or OnProjectileOverlap to override collision functionality

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// ---------------------------------------------------------
};

