// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Core/Abilities/CustomGameplayEffectContext.h"
#include "GA_MainGameplayAbility.generated.h"

class AMainCharacter;
class AMainPlayerController;
class UASC_AbilitySystemComponent;
class USkeletalMeshComponent;

#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)																				\
{																																						\
	if (!ensure(IsInstantiated()))																														\
	{																																					\
		return ReturnValue;																																\
	}																																					\
}


USTRUCT()
struct PROJECT0_API FAbilityMeshMontage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class USkeletalMeshComponent* Mesh;

	UPROPERTY()
	class UAnimMontage* Montage;

	FAbilityMeshMontage() : Mesh(nullptr), Montage(nullptr)
	{
	}

	FAbilityMeshMontage(class USkeletalMeshComponent* InMesh, class UAnimMontage* InMontage)
		: Mesh(InMesh), Montage(InMontage)
	{
	}
};

/**
 * EP0AbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EP0AbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 * EP0AbilityActivationGroup
 *
 *	Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EP0AbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX UMETA(Hidden)
};

/**
 * The base gameplay ability class used by this project.
 */
UCLASS()
class PROJECT0_API UGA_MainGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_MainGameplayAbility();



	///////// UTILITIES ////////
	UFUNCTION(BlueprintCallable, Category = "Project0|Ability")
	UASC_AbilitySystemComponent* GetP0AbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Project0|Ability")
	AMainPlayerController* GetP0PlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Project0|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Project0|Ability")
	AMainCharacter* GetP0CharacterFromActorInfo() const;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	UFUNCTION(BlueprintCallable, Category = "Project0|Ability")
	virtual bool TryCancelAbilitiesWithTags(const FGameplayTagContainer AbilitiesToCancel);
	
protected:
	/*
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CreateAbilityWidget(EHeroSkillsType HierarchicalSkillType);
	virtual void CreateAbilityWidget_Implementation(EHeroSkillsType HierarchicalSkillType);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveAbilityWidget();
	virtual void RemoveAbilityWidget_Implementation();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Project0|Ability")
	TSubclassOf<UBL_AbilityWidget> AbilityWidgetClass;
	
	TWeakObjectPtr<UBL_AbilityWidget> AbilityWidget;
	*/
	
	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY()
	FGameplayTagContainer TempCooldownTags;

	
	UPROPERTY(BlueprintReadWrite, Category = "Project0|Cooldown")
	FScalableFloat CooldownDuration;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Project0|Cooldown")
	FGameplayTagContainer CooldownTags;
	
public:
	//GAMEPLAY ABILITY INTERFACE// 

	
	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.

	virtual FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								   const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr,
	                                OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                         const FGameplayAbilityActivationInfo ActivationInfo,
	                         FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	                         const FGameplayEventData* TriggerEventData) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void BPreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo,
	                  const FGameplayAbilityActivationInfo ActivationInfo);
	virtual void BPreActivate_Implementation(const FGameplayAbilitySpecHandle Handle,
	                                         const FGameplayAbilityActorInfo& ActorInfo,
	                                         const FGameplayAbilityActivationInfo ActivationInfo);
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	// Allows C++ and Blueprint abilities to override how cost is checked in case they don't use a GE like weapon ammo
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	bool BCheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;
	virtual bool BCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle,
	                                       const FGameplayAbilityActorInfo& ActorInfo) const;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;
	// Allows C++ and Blueprint abilities to override how cost is applied in case they don't use a GE like weapon ammo
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void BApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo,
	                const FGameplayAbilityActivationInfo ActivationInfo) const;

	virtual void BApplyCost_Implementation(const FGameplayAbilitySpecHandle Handle,
	                                       const FGameplayAbilityActorInfo& ActorInfo,
	                                       const FGameplayAbilityActivationInfo ActivationInfo) const
	{
	};

	virtual void OnPawnAvatarSet();

	UFUNCTION(BlueprintCallable, Category= Ability, DisplayName= "CanActivateAbility")
	bool CanAbilityBeActivated();

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

	// Sends TargetData from the client to the Server and creates a new Prediction Window
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual void SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData);

	// Attempts to activate the given ability handle and batch all RPCs into one. This will only batch all RPCs that happen
	// in one frame. Best case scenario we batch ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);

	// Same as calling K2_EndAbility. Meant for use with batching system to end the ability externally.
	virtual void ExternalEndAbility();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UGA_MainGameplayAbility* GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent,
	                                                                FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UGA_MainGameplayAbility* GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent,
	                                                               TSubclassOf<UGameplayAbility> InAbilityClass);


	EP0AbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	EP0AbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

#pragma region Animations

	/** Returns the currently playing montage for this ability, if any */
	UFUNCTION(BlueprintCallable, Category = Animation)
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	/** Call to set/get the current montage from a montage task. Set to allow hooking up montage events to ability events */
	virtual void SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, class UAnimMontage* InCurrentMontage);

protected:
	///ANIMATIONS///
	/** Active montages being played by this ability */
	UPROPERTY()
	TArray<FAbilityMeshMontage> CurrentAbilityMeshMontages;

	bool FindAbillityMeshMontage(USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMontage);

	/** Immediately jumps the active montage to a section */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation")
	void MontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName);

	/** Sets pending section on active montage */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation")
	void MontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName);

	/**
	 * Stops the current animation montage.
	 *
	 * @param OverrideBlendOutTime If >= 0, will override the BlendOutTime parameter on the AnimMontage instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation", Meta = (AdvancedDisplay = "OverrideBlendOutTime"))
	void MontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime = -1.0f);

	/**
	* Stops all currently animating montages
	*
	* @param OverrideBlendOutTime If >= 0, will override the BlendOutTime parameter on the AnimMontage instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation", Meta = (AdvancedDisplay = "OverrideBlendOutTime"))
	void MontageStopForAllMeshes(float OverrideBlendOutTime = -1.0f);

#pragma endregion

public:
/*
	// If true, only activate this ability if the weapon that granted it is the currently equipped weapon.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bSourceObjectMustEqualCurrentWeaponToActivate;

	// camera mode manually set in the ability.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBCameraMode> MyCameraMode;
	// Current camera mode set by the ability.
	TSubclassOf<UBCameraMode> ActiveCameraMode;


	// Sets the ability's camera mode.
	UFUNCTION(BlueprintCallable, Category = "Project0|Ability")
	void SetCameraMode(TSubclassOf<UBCameraMode> CameraMode);

	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	UFUNCTION(BlueprintCallable, Category = "Project0|Ability")
	void ClearCameraMode();
*/
protected:
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project0|Ability Activation")
	EP0AbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project0|Ability Activation")
	EP0AbilityActivationGroup ActivationGroup;
};
