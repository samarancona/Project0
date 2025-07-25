// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BasePlayerState.h"

#include "Core/Abilities/AttributeSetBase.h"
#include "Core/Components/ASC_AbilitySystemComponent.h"

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UASC_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));


	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	SetNetUpdateFrequency(100);
}


TObjectPtr<UASC_AbilitySystemComponent> ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TObjectPtr<UAttributeSetBase>& ABasePlayerState::GetAttributeSetBase()
{
	return AttributeSetBase;
}

