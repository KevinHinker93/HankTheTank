// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "../ShootingSystem/ShotType.h"
#include "../ShootingSystem/BulletProjectile.h"
#include "ShootingComponent.generated.h"

class USoundBase;

/**
* Component class that handles the ability of an entity to fire specific shot types.
* All shot types share the same cooldown and audio for simplicity.
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class HANKTHETANK_API UShootingComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UShootingComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShootSettings")
		float fShootingInterval = 1.0f;

	/**
	* Called when an entity wants to trigger a single shot of a projectile.
	* @param ShotType - Defines the shot type, to determine which bullet should be fired.
	*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void InvokeShotByShotType(const EShotType ShotType);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
		USoundBase* Shotsound;

	UPROPERTY(EditDefaultsOnly, Category = "Shots", meta = (AllowPrivateAccess = "true"))
		TMap<EShotType, TSubclassOf<ABulletProjectile>> ProjectileClassByShotType;

	bool bCanShoot = true;

	void Shoot(const TSubclassOf<ABulletProjectile> ProjectileClass);

	void StartCooldown();
	UFUNCTION()
		void OnCooldownEnd();
};
