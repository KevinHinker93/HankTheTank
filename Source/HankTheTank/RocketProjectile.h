// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HankTheTankProjectile.h"
#include "RocketProjectile.generated.h"

class USphereComponent;
// TODO: rename to missile projectile
UCLASS()
class HANKTHETANK_API ARocketProjectile : public AHankTheTankProjectile
{
	GENERATED_BODY()

	ARocketProjectile();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float fStartingVelocity = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float fMaxVelocity = 1800.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float fTimeToReachMaxVelocityInSeconds = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float fExplosionRadius = 3.0f;

	/**
	* Can be called to let the projectile home to a specified target.
	*/
	UFUNCTION(BlueprintCallable)
		void OnStartHoming(AActor* HomingTarget);

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Collider, meta = (AllowPrivateAccess = "true"))
		USphereComponent* HomingDetectionTriggerComponent;

	UPROPERTY()
		AActor* CurrentHomingTargetActor = nullptr;

	UFUNCTION()
		void OnDetectionTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void SetProjectileVelocity(const float fVelocity);

	/**
	* Called to check if a new target is the nearest target.
	* @warning Only call this function if Target is checked for nullptr.
	*/
	bool IsNearestTarget(const AActor* Target);

	/**
	* Called to check if a Target is blocked by a static object.
	* @warning Only call this function if Target is checked for nullptr.
	*/
	bool IsTargetBlockedByAnObstacle(const AActor* Target);
};
