// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/P0_PlayerState.h"

#include "Core/Abilities/P0_AttributeSetBase.h"
#include "Core/Components/P0_AbilitySystemComponent.h"

AP0_PlayerState::AP0_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UP0_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);


	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UP0_AttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	SetNetUpdateFrequency(100);
}


TObjectPtr<UP0_AbilitySystemComponent> AP0_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TObjectPtr<UP0_AttributeSetBase>& AP0_PlayerState::GetAttributeSetBase()
{
	return AttributeSetBase;
}

