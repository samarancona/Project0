// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/P0_AbilitySet.h"
#include "Core/Components/P0_AbilitySystemComponent.h"
#include "Utils/P0_Macros.h"

void FP0AbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FP0AbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FP0AbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

TArray<TObjectPtr<UAttributeSet>> FP0AbilitySet_GrantedHandles::GetCurrentAttributeSets()
{
	return GrantedAttributeSets;
}

void FP0AbilitySet_GrantedHandles::TakeFromAbilitySystem(UP0_AbilitySystemComponent* ASC)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		ASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

void UP0_AbilitySet::GiveToAbilitySystem(UP0_AbilitySystemComponent* ASC, FP0AbilitySet_GrantedHandles* OutGrantedHandles,
                                        UObject* SourceObject) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FP0AbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			P0_LOG_WARNING("GrantedGameplayAbilities[%d] on ability set [%s] is not valid.", AbilityIndex,
			               *GetNameSafe(this));
			continue;
		}

		UP0_GameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UP0_GameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	//FOR NOW WE ADD THE ATTRIBUTE SET DIRECTLY IN THE CONSTRUCTOR OF PLAYER STATE
	
	// // Grant the attribute sets.
	// for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	// {
	// 	const FP0AbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];
	//
	// 	if (!IsValid(SetToGrant.AttributeSet))
	// 	{
	// 		BL_LOG_WARNING("GrantedGameplayAbilities[%d] on ability set [%s] is not valid.", SetIndex,
	// 					   *GetNameSafe(this));
	// 		continue;
	// 	}
	//
	// 	UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
	// 	ASC->AddAttributeSetSubobject(NewSet);
	//
	// 	if (OutGrantedHandles)
	// 	{
	// 		OutGrantedHandles->AddAttributeSet(NewSet);
	// 	}
	// }
	
	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FP0AbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			P0_LOG_WARNING("GrantedGameplayAbilities[%d] on ability set [%s] is not valid.", EffectIndex,
			               *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(
			GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
	
	
}
