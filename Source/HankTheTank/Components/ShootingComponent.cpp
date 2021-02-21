// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingComponent.h"
#include "../Utility/LogCategoryDefinitions.h"
#include "../Utility/StaticHelperFunctions.h"
#include "Kismet/GameplayStatics.h"

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

void UShootingComponent::Shoot(const TSubclassOf<ABulletProjectile> ProjectileClass)
{
	if (ProjectileClass && GetWorld())
	{
		FActorSpawnParameters spawnParameters;
		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector SpawnLocation = GetComponentLocation();
		FRotator SpawnRotation = GetComponentRotation();
		GetWorld()->SpawnActor<ABulletProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, spawnParameters);

		if (Shotsound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Shotsound, SpawnLocation);
		}

		StartCooldown();
	}
	else if (GetOwner())
	{
		UE_LOG(LogPlayerTank, Warning, TEXT("%s could not fire projectile, because projectile class is null or world is null."), *GetOwner()->GetName());
	}
}

void UShootingComponent::StartCooldown()
{
	if (GetOwner())
	{
		EXECUTE_BLOCK_CHECKED(GetWorld(), LogPlayerTank, TEXT("Could not start cooldown for tank %s, because world was null"), *GetOwner()->GetName())
		{
			bCanShoot = false;

			FTimerDelegate CooldownDelegate = FTimerDelegate::CreateUObject(this, &UShootingComponent::OnCooldownEnd);
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, CooldownDelegate, fShootingInterval, false);
		}
	}
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
			TSubclassOf<ABulletProjectile> ProjectileClass = ProjectileClassByShotType[ShotType];
			Shoot(ProjectileClass);
		}
		else if(GetOwner())
		{
			UE_LOG(LogPlayerTank, Warning, TEXT("%s could not fire projectile for specified shot type, have you forgot to set one?"), *GetOwner()->GetName());
		}
	}
}

