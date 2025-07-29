#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractorComponent.generated.h"

class IInteractableObjInterface;

/**
 * Component that finds and interacts with nearby actors implementing
 * the InteractableObjInterface.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT0_API UInteractorComponent : public UActorComponent
{
        GENERATED_BODY()

public:
        UInteractorComponent();

        virtual void BeginPlay() override;
        virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

        /** Radius used when searching for interactables */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
        float DetectionRadius;

        /** Max angle (in degrees) from forward vector to consider */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
        float MaxInteractAngle;

        /** Trace channel used for line traces */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
        TEnumAsByte<ECollisionChannel> TraceChannel;

        /** Returns the currently highlighted actor */
        UFUNCTION(BlueprintCallable, Category="Interaction")
        AActor* GetCurrentInteractable() const { return CurrentActor; }

        /** Attempts to interact with the current actor */
        UFUNCTION(BlueprintCallable, Category="Interaction")
        void TryInteract();

protected:
        void UpdateCurrentInteractable();

        UPROPERTY()
        AActor* CurrentActor;
};

