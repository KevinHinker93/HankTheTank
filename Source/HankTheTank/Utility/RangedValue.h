// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedValue.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct HANKTHETANK_API FRangedValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		float fMinValue;
	UPROPERTY(EditDefaultsOnly)
		float fMaxValue;

	float GetRandomValueInRange();
};
