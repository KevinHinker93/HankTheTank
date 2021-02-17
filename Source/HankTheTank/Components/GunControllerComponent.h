// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunControllerComponent.generated.h"

class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HANKTHETANK_API UGunControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGunControllerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fGunAngularVelocity;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FString sGunMeshName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* GunMeshToControll;

	/*void RotateTowardsTarget();
	void ReachedTargetRotation();*/
};
