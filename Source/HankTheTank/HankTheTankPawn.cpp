// Copyright Epic Games, Inc. All Rights Reserved.

#include "HankTheTankPawn.h"
#include "HankTheTankProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/GunControllerComponent.h"
#include "Components/TankTargetHandlerComponent.h"

const FName AHankTheTankPawn::MoveForwardBinding("MoveForward");
const FName AHankTheTankPawn::MoveRightBinding("MoveRight");
const FName AHankTheTankPawn::FireForwardBinding("FireForward");
const FName AHankTheTankPawn::FireRightBinding("FireRight");

AHankTheTankPawn::AHankTheTankPawn()
{	
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component

	

	/*TankRootSceneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TankRoot"));
	RootComponent = TankRootSceneComponent;

	TankMovementSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankMovementRoot"));
	TankMovementSceneComponent->SetupAttachment(RootComponent);*/

	TankMovementMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankMovementMesh"));
	RootComponent = TankMovementMeshComponent;
	//TankMovementMeshComponent->SetupAttachment(TankMovementSceneComponent);

	TankTowerSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankTowerRoot"));
	TankTowerSceneComponent->SetupAttachment(RootComponent);

	TankTowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankTowerMesh"));
	TankTowerMeshComponent->SetupAttachment(TankTowerSceneComponent);

	TankGunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankGunMesh"));
	TankGunMeshComponent->SetupAttachment(TankTowerSceneComponent);


	/*ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);*/
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	//// Create a camera boom...
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when ship does
	//CameraBoom->TargetArmLength = 1200.f;
	//CameraBoom->SetRelativeRotation(FRotator(-80.f, 0.f, 0.f));
	//CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	//// Create a camera...
	//CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	//CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Create gun controller
	GunControllerComponent = CreateDefaultSubobject<UGunControllerComponent>(TEXT("GunController"));

	// Create target handler
	TargetHandlerComponent = CreateDefaultSubobject<UTankTargetHandlerComponent>(TEXT("TargetHandler"));

	// Movement
	MoveSpeed = 1000.0f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
}

void AHankTheTankPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);
}

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
		GunControllerComponent->PreserveOriginalTowerRotation(fTankTowerZRot);
	}
	
	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	// Try and fire a shot
	FireShot(FireDirection);
}

void AHankTheTankPawn::FireShot(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				// spawn the projectile
				World->SpawnActor<AHankTheTankProjectile>(SpawnLocation, FireRotation);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AHankTheTankPawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void AHankTheTankPawn::ShotTimerExpired()
{
	bCanFire = true;
}

