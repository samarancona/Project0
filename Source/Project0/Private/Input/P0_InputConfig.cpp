// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/P0_InputConfig.h"
#include "Utils/P0_Macros.h"

UP0_InputConfig::UP0_InputConfig(const FObjectInitializer& ObjectInitializer)
{
}


const UInputAction* UP0_InputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FP0_InputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		P0_LOG_WARNING("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s].", *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UP0_InputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FP0_InputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}