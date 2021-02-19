// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HankTheTankProjectile.h"
#include "RocketProjectile.generated.h"

class USphereComponent;

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

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Collider, meta = (AllowPrivateAccess = "true"))
		USphereComponent* HomingDetectionTriggerComponent;

	UFUNCTION()
		void OnDetectionTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnStartHoming(const AActor* HomingTarget);

	void SetProjectileVelocity(const float fVelocity);
	
};
