// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GA_MainGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Character/AMainCharacter.h"
#include "Core/Components/ASC_AbilitySystemComponent.h"
#include "Player/MainPlayerController.h"


UGA_MainGameplayAbility::UGA_MainGameplayAbility()
{
	
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Default tags that block this ability from activating
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	
	// bSourceObjectMustEqualCurrentWeaponToActivate = false;

	ActivationPolicy = EP0AbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EP0AbilityActivationGroup::Independent;
}

UASC_AbilitySystemComponent* UGA_MainGameplayAbility::GetP0AbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UASC_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AMainPlayerController* UGA_MainGameplayAbility::GetP0PlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AMainPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UGA_MainGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}

	return nullptr;
}

AMainCharacter* UGA_MainGameplayAbility::GetP0CharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AMainCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}


void UGA_MainGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EP0AbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);
			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				bool test = ASC->TryActivateAbility(Spec.Handle,true);
			}
		}
	}
}

bool UGA_MainGameplayAbility::TryCancelAbilitiesWithTags(const FGameplayTagContainer AbilitiesToCancel)
{
	if(UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
	{
		ASC->CancelAbilities(&AbilitiesToCancel);
		return true;
	}
	return false;
}

/*
void UGA_MainGameplayAbility::CreateAbilityWidget_Implementation(EHeroSkillsType HierarchicalSkillType)
{
	if(GetControllerFromActorInfo()->IsLocalController())
	{
		if(AbilityWidget.IsValid())
		{
			if(!AbilityWidget->IsInViewport())
			{
				AbilityWidget->AddToViewport();
			}
			AbilityWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			AbilityWidget = CreateWidget<UBL_AbilityWidget>(GetWorld(), AbilityWidgetClass);
			AbilityWidget->HierarchicalSkillType = HierarchicalSkillType;
			AbilityWidget->AddToViewport();
		}
	}
}

void UGA_MainGameplayAbility::RemoveAbilityWidget_Implementation()
{
	if(AbilityWidget.IsValid())
	{
		AbilityWidget->RemoveFromParent();
	}
}*/

void UGA_MainGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.CoolDown")),
													   CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
FGameplayTagContainer* UGA_MainGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}


void UGA_MainGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UGA_MainGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_MainGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// ClearCameraMode();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_MainGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                           const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}
	
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	/*
	//Check to see if the ability is activating is related to the source object
	if (bSourceObjectMustEqualCurrentWeaponToActivate)
	{
		const AMainCharacter* Hero = Cast<AMainCharacter>(ActorInfo->AvatarActor);
		auto SourceOBJ = GetSourceObject(Handle, ActorInfo);
		const bool BisEqualtoSource = (UObject*)Hero->GetWeaponManager()->GetHoldingWeapon() == SourceOBJ;
		if (Hero && Hero->GetWeaponManager()->GetHoldingWeapon() && BisEqualtoSource)
		{
			return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
		}
		else
		{
			return false;
		}
	}*/
	return true;
}

void UGA_MainGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	K2_OnAbilityAdded();

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UGA_MainGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGA_MainGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	BPreActivate(Handle, *ActorInfo, ActivationInfo);
}

void UGA_MainGameplayAbility::BPreActivate_Implementation(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	
}



bool UGA_MainGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) && BCheckCost(Handle, *ActorInfo);
}

bool UGA_MainGameplayAbility::BCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo& ActorInfo) const
{
	return true;
}

void UGA_MainGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	BApplyCost(Handle,*ActorInfo,ActivationInfo);
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}


void UGA_MainGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvatarSet();
}

bool UGA_MainGameplayAbility::CanAbilityBeActivated()
{
	return CanActivateAbility(CurrentSpecHandle, CurrentActorInfo, nullptr, nullptr, nullptr);
}

void UGA_MainGameplayAbility::SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (IsPredictingClient())
	{
		UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
		check(ASC);

		FScopedPredictionWindow	ScopedPrediction(ASC, IsPredictingClient());

		FGameplayTag ApplicationTag; // Fixme: where would this be useful?
		CurrentActorInfo->AbilitySystemComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle,
			CurrentActivationInfo.GetActivationPredictionKey(), TargetData, ApplicationTag, ASC->ScopedPredictionKey);
	}
}

bool UGA_MainGameplayAbility::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately)
{
	UASC_AbilitySystemComponent* GSASC = Cast<UASC_AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (GSASC)
	{
		return GSASC->BatchRPCTryActivateAbility(InAbilityHandle, EndAbilityImmediately);
	}

	return false;
}

void UGA_MainGameplayAbility::ExternalEndAbility()
{
	check(CurrentActorInfo);

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UGA_MainGameplayAbility* UGA_MainGameplayAbility::GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UGA_MainGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

UGA_MainGameplayAbility* UGA_MainGameplayAbility::GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InAbilityClass);
		if (AbilitySpec)
		{
			return Cast<UGA_MainGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

UAnimMontage* UGA_MainGameplayAbility::GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh)
{
	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		return AbilityMeshMontage.Montage;
	}

	return nullptr;
}

void UGA_MainGameplayAbility::SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* InCurrentMontage)
{
	ensure(IsInstantiated());

	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		AbilityMeshMontage.Montage = InCurrentMontage;
	}
	else
	{
		CurrentAbilityMeshMontages.Add(FAbilityMeshMontage(InMesh, InCurrentMontage));
	}
}

bool UGA_MainGameplayAbility::FindAbillityMeshMontage(USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMeshMontage)
{
	for (FAbilityMeshMontage& MeshMontage : CurrentAbilityMeshMontages)
	{
		if (MeshMontage.Mesh == InMesh)
		{
			InAbilityMeshMontage = MeshMontage;
			return true;
		}
	}

	return false;
}


void UGA_MainGameplayAbility::MontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName)
{
	check(CurrentActorInfo);

	UASC_AbilitySystemComponent* const AbilitySystemComponent = Cast<UASC_AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Ensured());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageJumpToSectionForMesh(InMesh, SectionName);
	}
}

void UGA_MainGameplayAbility::MontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName)
{
	check(CurrentActorInfo);

	UASC_AbilitySystemComponent* const AbilitySystemComponent = Cast<UASC_AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Ensured());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageSetNextSectionNameForMesh(InMesh, FromSectionName, ToSectionName);
	}
}

void UGA_MainGameplayAbility::MontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UASC_AbilitySystemComponent* const AbilitySystemComponent = Cast<UASC_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
	if (AbilitySystemComponent != nullptr)
	{
		// We should only stop the current montage if we are the animating ability
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->CurrentMontageStopForMesh(InMesh, OverrideBlendOutTime);
		}
	}
}

void UGA_MainGameplayAbility::MontageStopForAllMeshes(float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UASC_AbilitySystemComponent* const AbilitySystemComponent = Cast<UASC_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
	if (AbilitySystemComponent != nullptr)
	{
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->StopAllCurrentMontages(OverrideBlendOutTime);
		}
	}
}


/*
void UGA_MainGameplayAbility::SetCameraMode(TSubclassOf<UBCameraMode> CameraMode)
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(SetCameraMode, );

	if (ABL_BaseCharacter* HeroCharacter = Cast<ABL_BaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		HeroCharacter->SetAbilityCameraMode(CameraMode, CurrentSpecHandle);
		ActiveCameraMode = CameraMode;
	}
}

void UGA_MainGameplayAbility::ClearCameraMode()
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ClearCameraMode, );

	if (ActiveCameraMode)
	{
		if (ABL_BaseCharacter* HeroCharacter = Cast<ABL_BaseCharacter>(GetAvatarActorFromActorInfo()))
		{
			HeroCharacter->ClearAbilityCameraMode(CurrentSpecHandle);
		}

		ActiveCameraMode = nullptr;
	}
}*/

