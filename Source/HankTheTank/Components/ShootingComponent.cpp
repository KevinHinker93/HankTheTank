// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingComponent.h"
#include "../Utility/LogCategoryDefinitions.h"

UShootingComponent::UShootingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UShootingComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UShootingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UShootingComponent::StartCooldown()
{
	bCanShoot = false;

	FTimerDelegate CooldownDelegate = FTimerDelegate::CreateUObject(this, &UShootingComponent::OnCooldownEnd);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, CooldownDelegate, fShootingInterval, false);
}

void UShootingComponent::OnCooldownEnd()
{
	bCanShoot = true;
}

void UShootingComponent::InvokeShotByShotType(const EShotType ShotType)
{
	// Only fire projectile if allowed to
	if (bCanShoot)
	{
		if (ProjectileClassByShotType.Contains(ShotType))
		{
			TSubclassOf<AHankTheTankProjectile> ProjectileClass = ProjectileClassByShotType[ShotType];

			if (ProjectileClass && GetWorld())
			{
				FActorSpawnParameters spawnParameters;
				spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				GetWorld()->SpawnActor<AHankTheTankProjectile>(ProjectileClass, GetComponentLocation(), GetComponentRotation(), spawnParameters);
				StartCooldown();
			}
			else if (GetOwner())
			{
				UE_LOG(LogPlayerTank, Warning, TEXT("%s could not fire projectile, because projectile class is null or world is null."), *GetOwner()->GetName());
			}

		}
		else if(GetOwner())
		{
			UE_LOG(LogPlayerTank, Warning, TEXT("%s could not fire projectile for specified shot type, have you forgot to set one?"), *GetOwner()->GetName());
		}
	}
}

