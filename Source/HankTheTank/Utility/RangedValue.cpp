// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedValue.h"

float FRangedValue::GetRandomValueInRange()
{
	return FMath::RandRange(fMinValue, fMaxValue);
}
