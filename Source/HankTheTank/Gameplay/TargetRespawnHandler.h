// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingTarget.h"
#include "TargetRespawnHandler.generated.h"

class UNavigationSystemV1;

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
		float fRespawnTime = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		float fSpawnRadiusFromObstacles = 5.0f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
		int iMaxSpawnIterations = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", meta = (AllowPrivateAccess = "true"))
		FName IgnoredActorTagForSpawning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		bool bDrawSpawnLocations = false;

	UPROPERTY()
		TArray<AShootingTarget*> SpawnedTargets;

	UPROPERTY()
		UNavigationSystemV1* NavMeshForSpawning;

	UFUNCTION()
		void OnRespawnATarget();

	FActorSpawnParameters SpawnParams;

	void SpawnTarget();
	void SetTargetFacingDirection(AShootingTarget* Target);

	UFUNCTION()
		void OnTargetDestroyed(AActor* DestroyedActor);
};
