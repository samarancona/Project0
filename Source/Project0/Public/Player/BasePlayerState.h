// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

class UAttributeSetBase;
class UASC_AbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECT0_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

	ABasePlayerState();
	
public:
	TObjectPtr<UASC_AbilitySystemComponent> GetAbilitySystemComponent() const;
	TObjectPtr<UAttributeSetBase>& GetAttributeSetBase();

protected:
	/* Ability System */

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UASC_AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSetBase> AttributeSetBase;
};
