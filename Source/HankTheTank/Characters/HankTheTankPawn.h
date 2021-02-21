// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HankTheTankPawn.generated.h"

class UGunControllerComponent;
class UTankTargetHandlerComponent;
class UStaticMeshComponent;
class USceneComponent;
class UBoxComponent;
class UShootingComponent;

UCLASS(Blueprintable)
class AHankTheTankPawn : public APawn
{
	GENERATED_BODY()

public:
	AHankTheTankPawn();

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;

protected:
	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireMissileBinding;
	static const FName FireRocketBinding;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		USceneComponent* TankTowerSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* TankTowerMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* TankGunMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		UGunControllerComponent* GunControllerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		UTankTargetHandlerComponent* TargetHandlerComponent;

	// ---------------------------------------

	// Action key binding functions
	UFUNCTION()
		void OnTriggerLeftClickFire();

	UFUNCTION()
		void OnTriggerRightClickFire();

	// --------------------------------------

	UPROPERTY()
		UShootingComponent* ShootingComponent;
};

