// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/CharacterBase.h"

#include "EnhancedInputSubsystems.h"
#include "Core/Abilities/P0_AbilitySet.h"
#include "Core/Abilities/P0_AttributeSetBase.h"
#include "Input/P0_InputComponent.h"
#include "Player/P0_PlayerController.h"
#include "Player/P0_PlayerState.h"


class UP0_InputComponent;
class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AP0_PlayerState* PS = GetPlayerState<AP0_PlayerState>())
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = CastChecked<UP0_AbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AttributeSetBase = PS->GetAttributeSetBase();

		SetMaxHealth(1000);
		SetHealth(GetMaxHealth());
		SetMaxShield(GetMaxHealth());
		SetShield(0);
		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(FGameplayTag::RequestGameplayTag("State.Dead"), 0);

		// Forcibly set the LooseTags count to 0		
		AbilitySystemComponent->SetLooseGameplayTagCount(FGameplayTag(), 0);
	}
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AP0_PlayerState* PS = GetPlayerState<AP0_PlayerState>())
	{
		AbilitySystemComponent = CastChecked<UP0_AbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AttributeSetBase = PS->GetAttributeSetBase();

		//// Set Health/Mana/Stamina/moveSPeed to their max. This is only necessary for *Respawn*.
		SetMaxHealth(1000);
		SetHealth(GetMaxHealth());
		SetMaxShield(GetMaxHealth());
		SetShield(0);

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void ACharacterBase::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->
		CharacterAbilitiesGiven)
	{
		return;
	}
	
	AbilitySystemComponent->ClearAllAbilities();
	
	auto OUTAbilitySetHandle = new FP0AbilitySet_GrantedHandles();
	auto ASC = CastChecked<UP0_AbilitySystemComponent>(AbilitySystemComponent);
	for (const UP0_AbilitySet* StartupAbility : AbilitySets)
	{
		if (StartupAbility)
		{
			StartupAbility->GiveToAbilitySystem(ASC, OUT OUTAbilitySetHandle);
			// auto grant = OutGratedAtt->GetCurrentAttributeSets();
		}
	}
	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}





bool ACharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

float ACharacterBase::GetHealth() const
{
	if (AttributeSetBase)
		return AttributeSetBase->GetHealth();

	return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase)
		return AttributeSetBase->GetMaxHealth();

	return 0.0f;
}

void ACharacterBase::SetHealth(const float Health) const
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void ACharacterBase::SetMaxHealth(const float NewMaxHealth) const
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetMaxHealth(NewMaxHealth);
	}
}

void ACharacterBase::SetShield(const float Shield) const
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetShield(Shield);
	}
}

void ACharacterBase::SetMaxShield(const float NewMaxShield) const
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetMaxShield(NewMaxShield);
	}
}

float ACharacterBase::GetShield() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetShield();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxShield() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxShield();
	}

	return 0.0f;
}


