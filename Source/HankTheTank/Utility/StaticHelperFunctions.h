// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StaticHelperFunctions.generated.h"

#define GET_OBJ_CHECKED(VariableToAssignValue, FunctionThatAssigns, LogCategory, Text, ...) \
	VariableToAssignValue = FunctionThatAssigns; \
	if(VariableToAssignValue == nullptr) \
		UE_LOG(LogCategory, Warning, Text, ##__VA_ARGS__)

// TODO: maybe not needed
// Assigns array and prints a message if the assigned array has size 0
#define GET_ARR_CHECKED(ArrToAssignValue, FunctionThatAssigns, LogCategory, Text, ...) \
	static_assert(TIsTArray<decltype(ArrToAssignValue)>::Value, "Arr must be TArray."); \
	ArrToAssignValue = FunctionThatAssigns; \
	if(ArrToAssignValue.Num() <= 0) \
	{ \
		UE_LOG(LogCategory, Warning, Text, ##__VA_ARGS__) \
	}
	

// Macro for executing a block only if the variable is not nullptr
// the execution block must be declared manually with {}
#define EXECUTE_BLOCK_CHECKED(CheckedVariable, LogCategory, Text, ...) \
	if(!CheckedVariable) \
	{ \
		UE_LOG(LogCategory, Warning, Text, ##__VA_ARGS__); \
	} \
	else 

// Macro for executing a function only if a variable is not nullptr
#define EXECUTE_FUNC_CHECKED(CheckedVariable, FuncToExecute, LogCategory, Text, ...) \
	if(!CheckedVariable) \
	{ \
		UE_LOG(LogCategory, Warning, Text, ##__VA_ARGS__); \
	} \
	else \
	{ \
		FuncToExecute; \
	}

UCLASS()
class HANKTHETANK_API UStaticHelperFunctions : public UObject
{
	GENERATED_BODY()
	
public:
	// Returns the desired component with the desired type, can be nullptr
	template <typename CompType>
	static CompType* GetActorCompAs(AActor* Owner)
	{
		CompType* DesiredComp = nullptr;
		if (Owner)
		{
			UActorComponent* ActorComp = Owner->GetComponentByClass(CompType::StaticClass());
			desiredComp = Cast<CompType>(ActorComp);
		}

		return DesiredComp;
	};

	// Returns the desired component with the desired type and a specific tag, can be empty
	template <typename CompType>
	static CompType* GetActorCompByTagAs(AActor* Owner, FName TagName)
	{
		TArray<UActorComponent*> Components;
		if (Owner)
			Components = Owner->GetComponentsByTag(CompType::StaticClass(), TagName);

		if (Components.Num() > 0)
			return Cast<CompType>(Components[0]);
		else
			return nullptr;
	};
};
