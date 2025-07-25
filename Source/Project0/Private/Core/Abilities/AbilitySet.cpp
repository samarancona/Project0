// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/AbilitySet.h"
#include "Core/Components/ASC_AbilitySystemComponent.h"
#include "Utils/CustomMacros.h"

void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

TArray<TObjectPtr<UAttributeSet>> FAbilitySet_GrantedHandles::GetCurrentAttributeSets()
{
	return GrantedAttributeSets;
}

void FAbilitySet_GrantedHandles::TakeFromAbilitySystem(UASC_AbilitySystemComponent* ASC)
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

void UAbilitySet::GiveToAbilitySystem(UASC_AbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles,
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
		const FAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			LOG_WARNING("GrantedGameplayAbilities[%d] on ability set [%s] is not valid.", AbilityIndex,
			               *GetNameSafe(this));
			continue;
		}

		UGA_MainGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGA_MainGameplayAbility>();

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
	// 	const FAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];
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
		const FAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			LOG_WARNING("GrantedGameplayAbilities[%d] on ability set [%s] is not valid.", EffectIndex,
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
