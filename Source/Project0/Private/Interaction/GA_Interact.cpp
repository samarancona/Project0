#include "Interaction/GA_Interact.h"
#include "Interaction/InteractorComponent.h"
#include "GameFramework/Actor.h"

UGA_Interact::UGA_Interact()
{
        ActivationPolicy = EP0AbilityActivationPolicy::OnInputTriggered;
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                   const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
        Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

        if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
        {
                EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
                return;
        }

        if (UInteractorComponent* Comp = ActorInfo->AvatarActor->FindComponentByClass<UInteractorComponent>())
        {
                Comp->TryInteract();
        }

        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

