#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Utils/CustomMacros.h"
#include "AttributeSetBase.generated.h"

/**
 * Base AttributeSet class to handle attributes such as Health, Shield, Stamina, etc.
 */
UCLASS()
class PROJECT0_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAttributeSetBase();

	// UAttributeSet interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	// Attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Stamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxStamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegenRate, Category = "Stamina")
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, StaminaRegenRate);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CharacterLevel, Category = "Character Level")
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, CharacterLevel);

	// Meta attribute used by DamageExecution
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Damage);

	UPROPERTY(BlueprintReadOnly, Category = "DamageResistance")
	FGameplayAttributeData DamageResistance;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, DamageResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "MoveSpeed")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MoveSpeed);

	// Adjust attribute for max changes (e.g., if MaxHealth changes, adjust current Health proportionally)
	void AdjustAttributeForMaxChange(
		const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty
	) const;

	// Replication notifies
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	UFUNCTION()
	void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel);

protected:
	FGameplayTag DeadTag;

	/** Helper: clamp new MoveSpeed value. */
	void ClampMoveSpeed(float& NewValue) const;

	/** Main damage handler function to encapsulate damage logic. */
	// void HandleDamageExecution(const struct FGameplayEffectModCallbackData& Data);

	/*/** Helper: finalize character death logic (firing events, awarding assists, etc.). #1#
	void HandleCharacterDeath(
		class ABL_BaseCharacter* Victim,
		class ABL_BaseCharacter* Killer,
		const struct FBLKillRecapProperties& KillRecap,
		float DamageDone,
		const FGameplayEffectSpec& EffectSpec
	);*/

	/** Helper: apply damage to shield first, then health. */
	// void ApplyDamageToShieldAndHealth(float& OutDamage, float DamageToApply);

	// Additional helper methods as needed...
};
