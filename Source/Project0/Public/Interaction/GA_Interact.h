#pragma once

#include "CoreMinimal.h"
#include "Core/Abilities/GA_MainGameplayAbility.h"
#include "GA_Interact.generated.h"

UCLASS()
class PROJECT0_API UGA_Interact : public UGA_MainGameplayAbility
{
        GENERATED_BODY()
public:
        UGA_Interact();

        virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData) override;
};

