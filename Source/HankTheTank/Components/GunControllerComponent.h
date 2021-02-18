// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunControllerComponent.generated.h"

class USceneComponent;
class UTankTargetHandlerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HANKTHETANK_API UGunControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGunControllerComponent();

	UPROPERTY(EditAnywhere, Category = "Rotation Settings", BlueprintReadWrite)
		float fGunAngularVelocity;
	// Angular threshold in degrees at which the tower starts rotating towards the target
	UPROPERTY(EditAnywhere, Category = "Rotation Settings", BlueprintReadWrite)
		float fGunAngularThreshold;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

	

	
	virtual void RotateTowardsTarget();
	//void ReachedTargetRotation();
};
