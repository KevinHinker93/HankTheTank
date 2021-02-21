// Copyright Epic Games, Inc. All Rights Reserved.

#include "HankTheTankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "../Utility/StaticHelperFunctions.h"
#include "../Utility/LogCategoryDefinitions.h"
#include "../Components/ShootingComponent.h"
#include "../ShootingSystem/ShotType.h"
#include "../Components/GunControllerComponent.h"
#include "../Components/TankTargetHandlerComponent.h"

const FName AHankTheTankPawn::MoveForwardBinding("MoveForward");
const FName AHankTheTankPawn::MoveRightBinding("MoveRight");
const FName AHankTheTankPawn::FireMissileBinding("FireMissile");
const FName AHankTheTankPawn::FireRocketBinding("FireRocket");

AHankTheTankPawn::AHankTheTankPawn()
{	
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankMovementMesh"));

	TankTowerSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankTowerRoot"));
	TankTowerSceneComponent->SetupAttachment(RootComponent);

	UStaticMeshComponent* TankTowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankTowerMesh"));
	TankTowerMeshComponent->SetupAttachment(TankTowerSceneComponent);

	UStaticMeshComponent* TankGunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankGunMesh"));
	TankGunMeshComponent->SetupAttachment(TankTowerSceneComponent);

	// Create gun controller
	GunControllerComponent = CreateDefaultSubobject<UGunControllerComponent>(TEXT("GunController"));

	// Create target handler
	TargetHandlerComponent = CreateDefaultSubobject<UTankTargetHandlerComponent>(TEXT("TargetHandler"));

	// Movement
	MoveSpeed = 1000.0f;
}

void AHankTheTankPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAction("FireMissile", IE_Released, this, &AHankTheTankPawn::OnTriggerLeftClickFire);
	PlayerInputComponent->BindAction("FireRocket", IE_Released, this, &AHankTheTankPawn::OnTriggerRightClickFire);
}

void AHankTheTankPawn::BeginPlay()
{
	Super::BeginPlay();

	GET_OBJ_CHECKED(ShootingComponent, UStaticHelperFunctions::GetActorCompAs<UShootingComponent>(this), LogPlayerTank,
		TEXT("Tank %s could not fetch shooting component, have you forgot to set one?"), *GetName());
}
// TODO: maybe with physics
void AHankTheTankPawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		const float fTankTowerZRot = TankTowerSceneComponent->GetComponentRotation().Euler().Z;
		const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);

		// Preserve gun tower rotation so it is facing same direction before a movement direction change
		EXECUTE_FUNC_CHECKED(GunControllerComponent, GunControllerComponent->PreserveOriginalTowerRotation(fTankTowerZRot), LogPlayerTank,
			TEXT("Tank %s could not preserve tower rotation, because GunControllerComponent was null, have you forgot to add one?"), *GetName());
	}
}

void AHankTheTankPawn::OnTriggerLeftClickFire()
{
	EXECUTE_FUNC_CHECKED(ShootingComponent, ShootingComponent->InvokeShotByShotType(EShotType::EBullet), LogPlayerTank,
		TEXT("Tank %s could not trigger left click shot, because ShootingComponent was null, have you forgot to add one?"), *GetName());
}

void AHankTheTankPawn::OnTriggerRightClickFire()
{
	EXECUTE_FUNC_CHECKED(ShootingComponent, ShootingComponent->InvokeShotByShotType(EShotType::EMissile), LogPlayerTank,
		TEXT("Tank %s could not trigger right click shot, because ShootingComponent was null, have you forgot to add one?"), *GetName());
}

