// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingTarget.h"
#include "../Utility/RangedValue.h"
#include "TargetRespawnHandler.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTargetSpawning, Log, All);

class UNavigationSystemV1;

/**
* Actor class responsible for spawning the targets the player needs to shoot.
* Uses the nav mesh to find suitable spawn locations.
* Conducts additional collision checks, for objects that do not affect the nav mesh,
* if a target will land on an invalid position.
*/

UCLASS()
class HANKTHETANK_API ATargetRespawnHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	ATargetRespawnHandler();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		int iMaxTargetCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		TSubclassOf<AShootingTarget> TargetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		FRangedValue fRespawnTimeRange;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// Radius at which the nav mesh should find a random point
	// Used to find a point in a more specific region instead of a complete random point on the nav mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
		float fNavMeshRandomPointRadius = 5.0f;
	// Bounding volume multiplier for the targets, so they might spawn farther away from each other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
		float fSpawnOverlapToleranceMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
		int iMaxSpawnIterations = 10;

	// Which object channels should interfere with target spawn position
	UPROPERTY(EditAnywhere, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
		TArray<TEnumAsByte<ECollisionChannel>> OverlappingObstacleChannels;

	// Which actors should be ignored for spawn collision checks
	UPROPERTY(EditInstanceOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
		TArray<AActor*> IgnoredActorsForSpawning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		bool bDrawSpawnLocations = false;

	UPROPERTY()
		TArray<AShootingTarget*> SpawnedTargets;

	UPROPERTY()
		UNavigationSystemV1* NavMeshForSpawning;

	UPROPERTY()
		TArray<TEnumAsByte<EObjectTypeQuery>> OverlappingObstacleObjectTypes;

	/**
	* Respawn timer delegate.
	*/
	UFUNCTION()
		void OnRespawnATarget();

	FActorSpawnParameters SpawnParams;

	/**
	* Called to spawn a single target.
	*/
	void SpawnTarget();

	/**
	* Called when a spawned target is destroyed.
	* Will trigger a respawn timer delegate to automatically respawn a target after timer depletion.
	*/
	UFUNCTION()
		void OnTargetDestroyed(AActor* DestroyedActor);

	/**
	* Checks if a target is blocked by obstacles using a sphere overlap.
	* @param SpawnLocation - Location to check for overlapping obstacles.
	* @param Target - The target actor to spawn, used for calculating overlap check radius.
	* @Warning - Use this function only if @param Target is checked for nullptr.
	*/
	bool IsSpawnLocationBlocked(const FVector SpawnLocation, const AShootingTarget* Target, TArray<AActor*>& OutBlockingActors);
	/**
	* Returns the farthest actor of a given list for the target.
	* @Warning Use this function only if @param Target is checked for nullptr.
	*/
	AActor* GetFarthestOverlappingActorFromTarget(const TArray<AActor*>& OverlappingActors, const AShootingTarget* Target);
	FVector CalculateNavMeshSearchStartingPointFromFarthestActor(const AActor* FarthestActor, const FVector CurrentTargetLocation);
	/**
	* @param bIsInValidPosition - Is the point to draw known to be an invalid spawn position, used to determine draw color.
	*/
	void DrawnDebugSpawnPoint(FVector PointLocation, bool bIsInValidPosition);
};
