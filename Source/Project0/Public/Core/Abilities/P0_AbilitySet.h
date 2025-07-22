// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "P0_GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "P0_AbilitySet.generated.h"


UENUM(BlueprintType)
enum class EP0_AbilityInputId :uint8
{
	None UMETA(DisplayName = "None"),
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel"),

	// Death UMETA(DispayName = "Death"),
	Jump UMETA (DisplayName = "Jump"),
	MovingBackward UMETA(DisplayName = "MovingBackward"),
	Sprint UMETA(DisplayName = "Sprint"),
	Crouch UMETA(DisplayName = "Crouch"),
	Attack UMETA(DisplayName = "Attack"),
	AimDownSight UMETA(DisplayName = "AimDownSight"),
	ShoulderSwap UMETA(DisplayName = "ShoulderSwap"),
	SwitchWeapon UMETA(DisplayName = "SwitchWeapon"),
	Reload UMETA(DisplayName = "Reload"),

	// Primary
	Ability1 UMETA(DisplayName = "Ability1"),
	// Secondary
	Ability2 UMETA(DisplayName = "Ability2"),
	// Ultimate
	Ability3 UMETA(DisplayName = "Ability3"),
};


/**
 * FBLAbilitySet_GameplayAbility
 *
 *	Data used by the ability set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct FP0AbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UP0_GameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * FP0AbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct FP0AbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * FP0AbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct FP0AbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

};


/**
 * FP0AbilitySet_GrantedHandles
 *
 *	Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct FP0AbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);
	
	TArray<TObjectPtr<UAttributeSet>> GetCurrentAttributeSets();
	
	void TakeFromAbilitySystem(UP0_AbilitySystemComponent* ASC);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/**
 * 
 */
UCLASS()
class PROJECT0_API UP0_AbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(UP0_AbilitySystemComponent* ASC, FP0AbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

	
protected:
	
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FP0AbilitySet_GameplayAbility> GrantedGameplayAbilities;

	
	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FP0AbilitySet_GameplayEffect> GrantedGameplayEffects;
	
	
	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FP0AbilitySet_AttributeSet> GrantedAttributes;
	
};
