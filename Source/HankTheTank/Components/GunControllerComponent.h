// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunControllerComponent.generated.h"

class USceneComponent;
class UTankTargetHandlerComponent;

/**
* Component class that handles the tower rotation for a tank entity.
* Could also be used for handling gun rotation.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HANKTHETANK_API UGunControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGunControllerComponent();

	// Angular velocity the tower rotates towards a target
	UPROPERTY(EditAnywhere, Category = "Rotation Settings", BlueprintReadWrite)
		float fGunAngularVelocity;
	// Angular threshold in degrees at which the tower starts rotating towards the target
	UPROPERTY(EditAnywhere, Category = "Rotation Settings", BlueprintReadWrite)
		float fGunAngularThreshold;
	// Tolerance at which the rotation of the tower will count as finished
	UPROPERTY(EditAnywhere, Category = "Rotation Settings", BlueprintReadWrite)
		float fAngularTolerance;

	/**
	* Called every frame to ensure the tower rotations remains the same although the owning actor's rotation has changed.
	*/
	void PreserveOriginalTowerRotation(const float fPreviousTankZTowerRot);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	* Called every frame to determine the angle between the tank tower and a specific target.
	* Will also set fCurrentDesiredZTowerRotation and fSign.
	*/
	virtual void SetCurrentAngleBetweenGunTowerAndTarget();

	bool bRotationIsDirty = false;
	float fCurrentDesiredZTowerRotation = 0.0f;
	float fSign = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		bool bDebugDrawLineFromTowerToMouse = false;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FName nGunTowerComponentTagName;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* GunTowerComponentToControl;

	UPROPERTY()
		UTankTargetHandlerComponent* TargetHandlerComponent;

	/**
	* Called every frame if bRotationIsDirty is true.
	* Will rotate the tank tower towards fCurrentDesiredZTowerRotation.
	*/
	virtual void RotateTowardsTarget();
};
