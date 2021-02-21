// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingTarget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTargets, Log, All);

class UStaticMeshComponent;

UCLASS()
class HANKTHETANK_API AShootingTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	AShootingTarget();

	void Die();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* TargetActorMeshComponent;
};
