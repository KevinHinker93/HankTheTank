// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetRespawnHandler.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "../Utility/StaticHelperFunctions.h"
#include "../Utility/LogCategoryDefinitions.h"
#include "Kismet/KismetMathLibrary.h"

ATargetRespawnHandler::ATargetRespawnHandler()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetRespawnHandler::BeginPlay()
{
	Super::BeginPlay();

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GET_OBJ_CHECKED(NavMeshForSpawning, UNavigationSystemV1::GetCurrent(this), LogPlayerTank,
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
	EXECUTE_BLOCK_CHECKED(NavMeshForSpawning, LogPlayerTank, TEXT("%s could not spawn target because nav mesh is null"), *GetName())
	{
		FNavLocation NavLocation;
		bool bFoundPoint = NavMeshForSpawning->GetRandomPoint(NavLocation);
		if (bFoundPoint)
		{
			FVector SpawnLocationOnNavMesh = NavLocation.Location;
			if (bDrawSpawnLocations)
			{
				FVector PointLoc = SpawnLocationOnNavMesh;
				PointLoc.Z += 10.0f;
				DrawDebugPoint(GetWorld(), PointLoc, 5.0f, FColor(0, 190, 0), true, 0.1f, 0);
			}

			AShootingTarget* ShootingTarget = GetWorld()->SpawnActor<AShootingTarget>(TargetClass, SpawnLocationOnNavMesh, FRotator::ZeroRotator, SpawnParams);

			bool bWasSpawnSuccessful = false;
			int iCurrentSpawnIteration = 0;
			while (bWasSpawnSuccessful == false && iCurrentSpawnIteration < iMaxSpawnIterations)
			{
				if (ShootingTarget)
				{
					FBox TargetBounds = ShootingTarget->GetComponentsBoundingBox();
					FVector AdjustedPlaceHeightPosition{ 0.0f, 0.0f, TargetBounds.GetExtent().Z + TargetBounds.Min.Z};
					ShootingTarget->AddActorWorldOffset(AdjustedPlaceHeightPosition);

					TArray<AActor*> PossibleOverlappingActors;
					ShootingTarget->GetOverlappingActors(PossibleOverlappingActors);

					if (PossibleOverlappingActors.Num() > 0 && !PossibleOverlappingActors[0]->ActorHasTag(IgnoredActorTagForSpawning))
					{
						if (bDrawSpawnLocations)
						{
							FVector PointLoc = ShootingTarget->GetActorLocation();
							DrawDebugPoint(GetWorld(), PointLoc, 5.0f, FColor(190, 0, 0), true, 0.1f, 0);
						}

						FVector SpawnedTargetToOverlappingActorVector = PossibleOverlappingActors[0]->GetActorLocation() - ShootingTarget->GetActorLocation();
						FVector NewSpawnOriginForNavMesh = ShootingTarget->GetActorLocation() - SpawnedTargetToOverlappingActorVector;
						FNavLocation NewNavLocation;
						NavMeshForSpawning->GetRandomPointInNavigableRadius(NewSpawnOriginForNavMesh, fSpawnRadiusFromObstacles, NewNavLocation);
						FVector NewTargetPosition{ NewNavLocation.Location.X, NewNavLocation.Location.Y, ShootingTarget->GetActorLocation().Z };
						ShootingTarget->SetActorLocation(NewTargetPosition);

						if (bDrawSpawnLocations)
						{
							FVector PointLoc = ShootingTarget->GetActorLocation();
							PointLoc.Z += 10.0f;
							DrawDebugPoint(GetWorld(), PointLoc, 5.0f, FColor(0, 190, 0), true, 0.1f, 0);
						}

						ShootingTarget->GetOverlappingActors(PossibleOverlappingActors);

						if (!(PossibleOverlappingActors.Num() > 0 && !PossibleOverlappingActors[0]->ActorHasTag(IgnoredActorTagForSpawning)))
						{
							bWasSpawnSuccessful = true;
						}
					}
					else
					{
						bWasSpawnSuccessful = true;
					}
				}

				++iCurrentSpawnIteration;
			}

			if (bWasSpawnSuccessful)
			{
				SetTargetFacingDirection(ShootingTarget);
				ShootingTarget->OnDestroyed.AddDynamic(this, &ATargetRespawnHandler::OnTargetDestroyed);
			}
			else
			{
				UE_LOG(LogPlayerTank, Warning, TEXT("%s could not find a suitable spawn location for a target"), *GetName());
				ShootingTarget->Destroy();
			}
		}
	}
}

void ATargetRespawnHandler::SetTargetFacingDirection(AShootingTarget* Target)
{
	FRotator LookAtRespawnHandlerRotation = UKismetMathLibrary::FindLookAtRotation(Target->GetActorLocation(), GetActorLocation());
	FVector TargetFacingDirectionVector = LookAtRespawnHandlerRotation.Euler();
	TargetFacingDirectionVector.X = 0.0f;
	TargetFacingDirectionVector.Y = 0.0f;
	Target->SetActorRotation(FRotator::MakeFromEuler(TargetFacingDirectionVector));
}

void ATargetRespawnHandler::OnTargetDestroyed(AActor* DestroyedActor)
{
	DestroyedActor->OnDestroyed.RemoveDynamic(this, &ATargetRespawnHandler::OnTargetDestroyed);

	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ATargetRespawnHandler::OnRespawnATarget);
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, RespawnDelegate, fRespawnTime, false);
}