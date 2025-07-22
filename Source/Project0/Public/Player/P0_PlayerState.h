// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "P0_PlayerState.generated.h"

class UP0_AttributeSetBase;
class UP0_AbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECT0_API AP0_PlayerState : public APlayerState
{
	GENERATED_BODY()

	AP0_PlayerState();
	
public:
	TObjectPtr<UP0_AbilitySystemComponent> GetAbilitySystemComponent() const;
	TObjectPtr<UP0_AttributeSetBase>& GetAttributeSetBase();

protected:
	/* Ability System */

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UP0_AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UP0_AttributeSetBase> AttributeSetBase;
};
