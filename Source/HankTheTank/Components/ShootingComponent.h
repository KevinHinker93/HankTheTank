// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ShootingComponent.generated.h"

class UHankTheTankProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HANKTHETANK_API UShootingComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UShootingComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// TODO: use enum for type
	 /**
	 * Called when an entity wants to trigger a single shot of a projectile.
	 * @param ShotType - Defines the shot type, to determine which bullet should be fired.
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void InvokeShotByShotType(FString ShotType);

private:
};
