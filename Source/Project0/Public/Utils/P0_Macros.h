// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Kismet/KismetSystemLibrary.h"
#include "CoreMinimal.h"

// Log
#define P0_LOG_INFO(format, ...) UE_LOG(LogTemp, Display, TEXT(format), ##__VA_ARGS__)
#define P0_LOG_WARNING(format, ...) UE_LOG(LogTemp, Warning, TEXT(format), ##__VA_ARGS__)
#define P0_LOG_ERROR(format, ...) UE_LOG(LogTemp, Error, TEXT(format), ##__VA_ARGS__)
#define P0_LOG_FATAL(format, ...) UE_LOG(LogTemp, Fatal, TEXT(format), ##__VA_ARGS__)


#define P0_LOG_ONSCREEN(x, ...)  \
UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT(x), ##__VA_ARGS__));

/**
 * Helps quickly requesting a Gameplay Tag avoiding the use of FGameplayTag::RequestGameplayTag
 */
#define P0_GAMEPLAYTAG(TagName) FGameplayTag::RequestGameplayTag(TagName)

// Uses macros from Attributes.h to create Attribute accessors
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
