// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetRespawnHandler.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "../Utility/StaticHelperFunctions.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTargetSpawning);

ATargetRespawnHandler::ATargetRespawnHandler()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetRespawnHandler::BeginPlay()
{
	Super::BeginPlay();

	for (auto CollisionChannel : OverlappingObstacleChannels)
	{
		OverlappingObstacleObjectTypes.Add(UEngineTypes::ConvertToObjectType(CollisionChannel));
	}

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GET_OBJ_CHECKED(NavMeshForSpawning, UNavigationSystemV1::GetCurrent(this), LogTargetSpawning,
		TEXT("%s could not find a nav mesh for spawning, have you forgot to add one?"), *GetName());

	for (int i = 0; i < iMaxTargetCount; ++i)
	{
		SpawnTarget();
	}
}

void ATargetRespawnHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetRespawnHandler::OnRespawnATarget()
{
	SpawnTarget();
}

void ATargetRespawnHandler::SpawnTarget()
{
	EXECUTE_BLOCK_CHECKED(NavMeshForSpawning, LogTargetSpawning, TEXT("%s could not spawn target because nav mesh is null"), *GetName())
	{
		// Create new target instance 

		FTransform TargetTransform;

		AShootingTarget* ShootingTarget = GetWorld()->SpawnActorDeferred<AShootingTarget>(TargetClass, TargetTransform, nullptr, nullptr,
			SpawnParams.SpawnCollisionHandlingOverride);

		if (ShootingTarget)
		{
			// Add new target instance to ignored list so it will not block any spawn collision checks
			IgnoredActorsForSpawning.Add(ShootingTarget);

			bool bWasSpawnSuccessful = false;
			int iCurrentSpawnIteration = 0;
			FVector FinalTargetPosition = FVector::ZeroVector;

			FNavLocation NavLocation;
			bool bFoundPoint = false;
			bool bIsSpawnLocationBlocked;
			TArray<AActor*> OverlappingActors;

			while (bWasSpawnSuccessful == false && iCurrentSpawnIteration < iMaxSpawnIterations)
			{
				// Fetch random location on nav mesh
				bFoundPoint = NavMeshForSpawning->GetRandomPoint(NavLocation);
				if (bFoundPoint)
				{
					FinalTargetPosition = NavLocation.Location;

					if (bDrawSpawnLocations)
					{
						DrawnDebugSpawnPoint(FinalTargetPosition, false);
					}

					bIsSpawnLocationBlocked = IsSpawnLocationBlocked(FinalTargetPosition, ShootingTarget, OverlappingActors);

					if (bIsSpawnLocationBlocked)
					{
						if (bDrawSpawnLocations)
						{
							DrawnDebugSpawnPoint(FinalTargetPosition, true);
						}

						// Try to move spawn location away from blocking actors and try finding a new random location in a specific radius

						AActor* FarthestBlockingActor = GetFarthestOverlappingActorFromTarget(OverlappingActors, ShootingTarget);
						if (FarthestBlockingActor)
						{
							FVector NavMeshOriginRequestPoint = CalculateNavMeshSearchStartingPointFromFarthestActor(FarthestBlockingActor, FinalTargetPosition);
							bFoundPoint = NavMeshForSpawning->GetRandomPointInNavigableRadius(NavMeshOriginRequestPoint, fNavMeshRandomPointRadius, NavLocation);

							if (bFoundPoint)
							{
								FinalTargetPosition = NavLocation.Location;

								if (bDrawSpawnLocations)
								{
									DrawnDebugSpawnPoint(FinalTargetPosition, false);
								}

								// Check if the corrected spawn location away from the original overlapping actors is valid
								// If not try to start finding a new complete radnom spawn location

								bIsSpawnLocationBlocked = IsSpawnLocationBlocked(FinalTargetPosition, ShootingTarget, OverlappingActors);

								if (!bIsSpawnLocationBlocked)
								{
									bWasSpawnSuccessful = true;
								}
								else
								{
									if (bDrawSpawnLocations)
									{
										DrawnDebugSpawnPoint(FinalTargetPosition, true);
									}
								}
							}
						}
					}
					else
					{
						bWasSpawnSuccessful = true;
					}
				}

				++iCurrentSpawnIteration;
			}

			IgnoredActorsForSpawning.Remove(ShootingTarget); // Remove the current target from the collision ignore list so it will still overlap newly spawned targets

			if (bWasSpawnSuccessful)
			{
				FTransform FinalTargetTransform = FTransform(FinalTargetPosition);
				UGameplayStatics::FinishSpawningActor(ShootingTarget, FinalTargetTransform);
				ShootingTarget->OnDestroyed.AddDynamic(this, &ATargetRespawnHandler::OnTargetDestroyed);
			}
			else
			{
				UE_LOG(LogTargetSpawning, Warning, TEXT("%s could not find a suitable spawn location for a target"), *GetName());
				ShootingTarget->Destroy();
			}
		}
	}
}

void ATargetRespawnHandler::OnTargetDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor)
	{
		DestroyedActor->OnDestroyed.RemoveDynamic(this, &ATargetRespawnHandler::OnTargetDestroyed);
	}

	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ATargetRespawnHandler::OnRespawnATarget);
	FTimerHandle Handle;
	float fRespawnTime = fRespawnTimeRange.GetRandomValueInRange();
	GetWorldTimerManager().SetTimer(Handle, RespawnDelegate, fRespawnTime, false);
}

bool ATargetRespawnHandler::IsSpawnLocationBlocked(const FVector SpawnLocation, const AShootingTarget* Target, TArray<AActor*>& OutBlockingActors)
{
	OutBlockingActors.Empty(); // Clear out hits, so it will only contain actors hit in this collision check
	float OverlapSphereRadius = Target->GetComponentsBoundingBox().GetExtent().X * fSpawnOverlapToleranceMultiplier; // Use X component, because targets are only spheres in this case

	return UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SpawnLocation, OverlapSphereRadius, OverlappingObstacleObjectTypes,
		nullptr, IgnoredActorsForSpawning, OutBlockingActors);
}

AActor* ATargetRespawnHandler::GetFarthestOverlappingActorFromTarget(const TArray<AActor*>& OverlappingActors, const AShootingTarget* Target)
{
	AActor* FarthestActor = nullptr;
	float fGreatestDistance = 0.0f;
	for (int i = 0; i < OverlappingActors.Num(); ++i)
	{
		AActor* OverlappingActor = OverlappingActors[i];
		if (OverlappingActor)
		{
			const FVector OverlappingActorToTargetDistVector = Target->GetActorLocation() - OverlappingActor->GetActorLocation();
			const float fDistance = OverlappingActorToTargetDistVector.SizeSquared2D();
			if (fDistance > fGreatestDistance)
			{
				fGreatestDistance = fDistance;
				FarthestActor = OverlappingActor;
			}
		}
	}

	return FarthestActor;
}

FVector ATargetRespawnHandler::CalculateNavMeshSearchStartingPointFromFarthestActor(const AActor* FarthestActor, const FVector CurrentTargetLocation)
{
	FVector FarthestActorLocation = FarthestActor->GetActorLocation();
	float fFarthesActorBoundingBoxMaxComponent = FarthestActor->GetComponentsBoundingBox().GetExtent().GetMax();

	FVector SpawnLocationToFarthestActorDirection = FarthestActorLocation - CurrentTargetLocation;
	float fSpawnLocationToFarthestActorDistance = SpawnLocationToFarthestActorDirection.Size2D();
	SpawnLocationToFarthestActorDirection.Normalize();

	// Calculate starting point for new random point on nav mesh
	// Starting position starts from the farthest blocking actor in the opposite direction
	// from the target and the farthest actor. This will ensure that the Starting location will
	// be away from all overlapped objects.
	FVector NavMeshOriginRequestPoint = FarthestActorLocation +
		(SpawnLocationToFarthestActorDirection * fSpawnLocationToFarthestActorDistance + fFarthesActorBoundingBoxMaxComponent);

	return NavMeshOriginRequestPoint;
}

void ATargetRespawnHandler::DrawnDebugSpawnPoint(FVector PointLocation, bool bIsInValidPosition)
{
	FColor PointColor{ 0,0,0 };
	if (bIsInValidPosition)
	{
		PointColor.R = 200;
	}
	else
	{
		PointColor.G = 200;
	}

	DrawDebugPoint(GetWorld(), PointLocation, 5.0f, PointColor, true, 0.1f, 0);
}
