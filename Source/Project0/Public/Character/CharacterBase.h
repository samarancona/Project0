// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputMappingContext.h"
#include "Core/Components/P0_AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Input/P0_InputConfig.h"
#include "CharacterBase.generated.h"

class UP0_AbilitySet;

UCLASS()
class PROJECT0_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable, Category = "Project0|Attributes")
	bool IsAlive() const;
	UFUNCTION(BlueprintCallable, Category = "Project0|Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Project0|Attributes")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Project0|Attributes")
	void SetHealth(float Health) const;
	UFUNCTION(BlueprintCallable, Category = "Project0|Attributes")
	void SetMaxHealth(float NewMaxHealth) const;
protected:
	
	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();
	
	UPROPERTY()
	TObjectPtr<UP0_AbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<class UP0_AttributeSetBase> AttributeSetBase;
	
	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project0|Abilities")
	TArray<TObjectPtr<UP0_AbilitySet>> AbilitySets;
};
