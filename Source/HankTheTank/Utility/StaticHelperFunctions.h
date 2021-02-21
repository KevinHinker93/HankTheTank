// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StaticHelperFunctions.generated.h"

/**
* Helper macro for assigning a pointer to a variable with a specific function.
* Will print a warning if the assigned pointer is nullptr.
* Use this macro where a log is useful e.g. to see if an assignment was successful.
* @param Text - The print text if the assigned pointer is nullptr, can be formatted with @param ... .
*/
#define GET_OBJ_CHECKED(VariableToAssignValue, FunctionThatAssigns, LogCategory, Text, ...) \
	VariableToAssignValue = FunctionThatAssigns; \
	if(VariableToAssignValue == nullptr) \
		UE_LOG(LogCategory, Warning, Text, ##__VA_ARGS__)

/**
* Helper macro for executing a block only if the specified variable is not nullptr.
* The execution block must be declared manually with {}
* Use this macro where a log is useful e.g. to see if a specific section is not executed.
* @param Text - The print text if the assigned pointer is nullptr, can be formatted with @param ... .
*/
#define EXECUTE_BLOCK_CHECKED(CheckedVariable, LogCategory, Text, ...) \
	if(!CheckedVariable) \
	{ \
		UE_LOG(LogCategory, Warning, Text, ##__VA_ARGS__); \
	} \
	else 

/**
* Helper macro for executing a function only if the specified variable is not nullptr.
* Use this macro where a log is useful e.g. to see if a specific function is not executed.
* @param Text - The print text if the assigned pointer is nullptr, can be formatted with @param ... .
*/
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
	/**
	* Returns the desired component with the desired type, can be nullptr.
	* @param Owner - The owner of the component.
	*/
	template <typename CompType>
	static CompType* GetActorCompAs(AActor* Owner)
	{
		CompType* DesiredComp = nullptr;
		if (Owner)
		{
			UActorComponent* ActorComp = Owner->GetComponentByClass(CompType::StaticClass());
			DesiredComp = Cast<CompType>(ActorComp);
		}

		return DesiredComp;
	};

	/**
	* Returns the desired component with the desired type and a specific tag, can be nullptr.
	* @param Owner - The owner of the component.
	* @param TagName - The tag which the searched component should have.
	*/
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
