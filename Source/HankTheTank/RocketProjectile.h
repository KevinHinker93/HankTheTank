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

protected:
	virtual void BeginPlay();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Collider, meta = (AllowPrivateAccess = "true"))
		USphereComponent* HomingDetectionTriggerComponent;

	UFUNCTION()
		void OnDetectionTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnStartHoming(const AActor* HomingTarget);
	
};
