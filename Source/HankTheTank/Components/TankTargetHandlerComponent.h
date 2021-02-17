// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankTargetHandlerComponent.generated.h"

class APlayerController;

/**
* Class that is responsible for tracking a target for a shooting entity.
* Target can either be an Actor or a location
* @warning Target Actor can be nullptr if the target can only be represented by a location e.g mouse position.
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HANKTHETANK_API UTankTargetHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTankTargetHandlerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	* Called every frame to track a specific target.
	*/
	virtual void TrackTarget();

	// Can be nullptr, always check for nullptr
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		AActor* TargetActor = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FVector TargetLocation = FVector::ZeroVector;

private:
	APlayerController* TankPlayerController;
};
