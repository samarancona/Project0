// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Abilities/P0_AttributeSetBase.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

UP0_AttributeSetBase::UP0_AttributeSetBase()
{
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void UP0_AttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate even if the replicated value is the same as the old value
	DOREPLIFETIME_CONDITION_NOTIFY(UP0_AttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UP0_AttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UP0_AttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UP0_AttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UP0_AttributeSetBase, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UP0_AttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UP0_AttributeSetBase, CharacterLevel, COND_None, REPNOTIFY_Always);
	
}

void UP0_AttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		// Clamp move speed within valid range
		ClampMoveSpeed(NewValue);
	}
}

void UP0_AttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Determine what attribute changed
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// HandleDamageExecution(Data);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
}

void UP0_AttributeSetBase::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue,
                                                 float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

/*
void UP0_AttributeSetBase::HandleDamageExecution(const FGameplayEffectModCallbackData& Data)
{
	float LocalDamageDone = FMath::Max(GetDamage(), 0.0f);
	SetDamage(0.0f);

	if (LocalDamageDone <= 0.f)
	{
		return;
	}

	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ABL_BaseCharacter* TargetCharacter = nullptr;
	ABL_BaseCharacter* InstigatorCharacter = Cast<
		ABL_BaseCharacter>(Data.EffectSpec.GetEffectContext().GetInstigator());

	// Fallback if Instigator is not found
	if (!InstigatorCharacter)
	{
		if (ABLPlayerState* InstigatorPS = Cast<ABLPlayerState>(Data.EffectSpec.GetEffectContext().GetInstigator()))
		{
			InstigatorCharacter = Cast<ABL_BaseCharacter>(InstigatorPS->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ABL_BaseCharacter>(TargetActor);
	}

	const bool bDamageSelf = (InstigatorCharacter == TargetCharacter);

	// Build kill recap struct
	UObject* EffectCauser = nullptr;
	AActor* EffectCauserActor = Data.EffectSpec.GetEffectContext().GetEffectCauser();
	if (EffectCauserActor && EffectCauserActor->GetClass()->ImplementsInterface(UBL_WeaponInfoProvider::StaticClass()))
	{
		EffectCauser = Data.EffectSpec.GetEffectContext().GetEffectCauser();
	}
	else
	{
		EffectCauser = Data.EffectSpec.GetEffectContext().GetSourceObject();
	}

	FBLKillRecapProperties KillRecap;
	KillRecap.WeaponCauser = EffectCauser;
	KillRecap.KillerName = InstigatorCharacter
		                       ? InstigatorCharacter->GetPlayerState()->GetPlayerName()
		                       : TEXT("Unknown");
	KillRecap.VictimName = TargetCharacter ? TargetCharacter->GetPlayerState()->GetPlayerName() : TEXT("Unknown");
	KillRecap.CauserSilhouette = KillRecap.GetWeaponSilhouette();
	KillRecap.bIsHeadShot = Data.EffectSpec.GetDynamicAssetTags().HasTag(
		FGameplayTag::RequestGameplayTag(FName("Effect.Damage.HeadShot"))
	);
	KillRecap.KillerTeamId = InstigatorCharacter ? InstigatorCharacter->GetTeamID() : -1;
	KillRecap.KillerHeroType = InstigatorCharacter
		                           ? InstigatorCharacter->GetPlayerState<ABLPlayerState>()->GetSelectedHero()
		                           : EHeroType::None;
	KillRecap.VictimHeroType = TargetCharacter
		                           ? TargetCharacter->GetPlayerState<ABLPlayerState>()->GetSelectedHero()
		                           : EHeroType::None;

	// Apply damage
	if (TargetCharacter)
	{
		const bool bWasAlive = TargetCharacter->IsAlive();

		ApplyDamageToShieldAndHealth(LocalDamageDone, LocalDamageDone);

		// If damaging self, it’s considered immediate “suicide”/self damage case
		if (bDamageSelf)
		{
			if (!TargetCharacter->IsAlive() &&
				!GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(DeadTag))
			{
				TargetCharacter->OnHeroKilled(InstigatorCharacter ? InstigatorCharacter->GetController() : nullptr,
				                              KillRecap, KillRecap.bIsHeadShot);

#if WITH_SERVER_CODE
				// Generate a death event
				FGameplayEventData Payload;
				Payload.EventTag = FGameplayTag::RequestGameplayTag("GameplayEvent.Death");
				Payload.OptionalObject = InstigatorCharacter;
				Payload.Target = TargetCharacter;
				Payload.OptionalObject2 = Data.EffectSpec.Def;
				Payload.ContextHandle = Data.EffectSpec.GetEffectContext();
				Payload.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
				Payload.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
				Payload.EventMagnitude = Data.EvaluatedData.Magnitude;

				FScopedPredictionWindow NewScopedWindow(TargetCharacter->GetAbilitySystemComponent(), true);
				TargetCharacter->GetBlAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
#endif
			}
			return;
		}

		// If the victim was alive before damage
		if (bWasAlive)
		{
			TargetCharacter->OnUpdateDamageReceivedStat(LocalDamageDone);

			if (const ABWeaponBase* HoldingWeapon = TargetCharacter->GetWeaponManager()->HoldingWeapon)
			{
				TargetCharacter->OnWeaponDamageReceivedHero(HoldingWeapon->WeaponType, LocalDamageDone);
			}

			// Register assist if an instigator exists
			if (InstigatorCharacter && InstigatorCharacter->GetPlayerState())
			{
				TargetCharacter->AddAssistedPlayer_Server(
					InstigatorCharacter->GetPlayerState()->GetPlayerName(),
					LocalDamageDone
				);
			}
		}

		// Update instigator stats
		if (InstigatorCharacter)
		{
			if (EffectCauser && EffectCauser->GetClass()->ImplementsInterface(UBL_WeaponInfoProvider::StaticClass()))
			{
				InstigatorCharacter->OnWeaponDamageGivenHero(KillRecap.GetWeaponType(), KillRecap.bIsHeadShot);
			}

			InstigatorCharacter->OnUpdateDamageGivenStat(LocalDamageDone);

			if (const UBL_BaseHeroSkills* HeroAbility = Cast<UBL_BaseHeroSkills>(
				Data.EffectSpec.GetEffectContext().GetAbility()))
			{
				InstigatorCharacter->OnAbilityDamageHero(HeroAbility->GetSkillType(), LocalDamageDone);
			}

			if (KillRecap.bIsHeadShot)
			{
				InstigatorCharacter->OnUpdateHeadShotStat();
			}
		}

		// Check if the target just died
		if (!TargetCharacter->IsAlive() &&
			!GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(DeadTag))
		{
			// Fallback for null instigator
			if (!InstigatorCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("EXCEPTION: Instigator is null"));
				if (ABLPlayerState* OwnerPS = Cast<ABLPlayerState>(Data.EffectSpec.GetEffectContext().GetInstigator()))
				{
					InstigatorCharacter = Cast<ABL_BaseCharacter>(OwnerPS->GetPawn());
				}
			}

			// HandleCharacterDeath(TargetCharacter, InstigatorCharacter, KillRecap, LocalDamageDone, Data.EffectSpec);
		}
	}
}
*/


void UP0_AttributeSetBase::ClampMoveSpeed(float& NewValue) const
{
	NewValue = FMath::Clamp<float>(NewValue, 150.0f, 1000.0f);
}

/*void UP0_AttributeSetBase::HandleCharacterDeath(
	ABL_BaseCharacter* Victim,
	ABL_BaseCharacter* Killer,
	const FBLKillRecapProperties& KillRecap,
	float DamageDone,
	const FGameplayEffectSpec& EffectSpec
)
{
	if (!Victim) return;

	Victim->OnHeroKilled(Killer ? Killer->GetController() : nullptr, KillRecap, KillRecap.bIsHeadShot);
	Victim->OnWeaponKillByHero();

	// Award kills to the instigator
	if (Killer)
	{
		if (const UBL_BaseHeroSkills* HeroAbility = Cast<
			UBL_BaseHeroSkills>(EffectSpec.GetEffectContext().GetAbility()))
		{
			Killer->OnAbilityKillHero(HeroAbility->GetSkillType());
		}

		Killer->OnWeaponKilledHero(KillRecap.GetWeaponType());

		// Award assists
		const TArray<FString> AssistedPlayersName = Victim->GetAssistedPlayersName(0.25f);
		if (AssistedPlayersName.Num() > 0 && GetWorld() && GetWorld()->GetGameState())
		{
			for (APlayerState* PS : GetWorld()->GetGameState()->PlayerArray)
			{
				if (ABLPlayerState* BLPS = Cast<ABLPlayerState>(PS))
				{
					if (Killer->GetPlayerState()->GetPlayerName() != BLPS->GetPlayerName() &&
						AssistedPlayersName.Contains(BLPS->GetPlayerName()))
					{
						BLPS->UpdateAssists();
						BLPS->ShowAssist(Victim->GetPlayerState()->GetPlayerName());
					}
				}
			}
		}
	}

	Victim->ClearAssistedPlayer_Server();

#if WITH_SERVER_CODE
	FGameplayEventData Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag("GameplayEvent.Death");
	Payload.OptionalObject = Killer;
	Payload.Target = Victim;
	Payload.OptionalObject2 = EffectSpec.Def;
	Payload.ContextHandle = EffectSpec.GetEffectContext();
	Payload.InstigatorTags = *EffectSpec.CapturedSourceTags.GetAggregatedTags();
	Payload.TargetTags = *EffectSpec.CapturedTargetTags.GetAggregatedTags();
	Payload.EventMagnitude = DamageDone;

	FScopedPredictionWindow NewScopedWindow(Victim->GetAbilitySystemComponent(), true);
	Victim->GetBlAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
#endif
}*/

/*void UP0_AttributeSetBase::ApplyDamageToShieldAndHealth(float& OutDamage, float DamageToApply)
{
	if (GetShield() > 0.f)
	{
		float NewShield = GetShield() - DamageToApply;
		SetShield(FMath::Clamp(NewShield, 0.0f, GetMaxShield()));

		// If damage still remains after shield
		float RemainingDamage = DamageToApply - (GetShield() > 0.f ? DamageToApply : (DamageToApply + NewShield));
		if (RemainingDamage > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth() - RemainingDamage, 0.0f, GetMaxHealth()));
		}
	}
	else
	{
		SetHealth(FMath::Clamp(GetHealth() - DamageToApply, 0.0f, GetMaxHealth()));
	}

	OutDamage = DamageToApply; // If you need to reuse the final value
}*/

void UP0_AttributeSetBase::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
                                                     const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
                                                     const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (AbilityComp && !FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue))
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float Delta = (CurrentMaxValue > 0.f)
			                    ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			                    : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, Delta);
	}
}

void UP0_AttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UP0_AttributeSetBase, Health, OldHealth);
}

void UP0_AttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UP0_AttributeSetBase, MaxHealth, OldMaxHealth);
}
void UP0_AttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UP0_AttributeSetBase, Stamina, OldStamina);
}

void UP0_AttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UP0_AttributeSetBase, MaxStamina, OldMaxStamina);
}

void UP0_AttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UP0_AttributeSetBase, StaminaRegenRate, OldStaminaRegenRate);
}

void UP0_AttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UP0_AttributeSetBase, MoveSpeed, OldMoveSpeed);
}

void UP0_AttributeSetBase::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UP0_AttributeSetBase, CharacterLevel, OldCharacterLevel);
}
