#include "Interaction/InteractorComponent.h"
#include "Interaction/InteractableObjInterface.h"
#include "Kismet/KismetSystemLibrary.h"

UInteractorComponent::UInteractorComponent()
{
        PrimaryComponentTick.bCanEverTick = true;
        DetectionRadius = 200.f;
        MaxInteractAngle = 45.f;
        TraceChannel = ECC_Visibility;
        CurrentActor = nullptr;
}

void UInteractorComponent::BeginPlay()
{
        Super::BeginPlay();
}

void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
        Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
        UpdateCurrentInteractable();
}

void UInteractorComponent::UpdateCurrentInteractable()
{
        AActor* Owner = GetOwner();
        if (!Owner) return;

        FVector OwnerLoc = Owner->GetActorLocation();
        TArray<AActor*> Actors;
        UKismetSystemLibrary::SphereOverlapActors(GetWorld(), OwnerLoc, DetectionRadius, {}, nullptr, {}, Actors);

        AActor* BestActor = nullptr;
        float BestDot = FMath::Cos(FMath::DegreesToRadians(MaxInteractAngle));

        for (AActor* TestActor : Actors)
        {
                if (!TestActor || !TestActor->GetClass()->ImplementsInterface(UInteractableObjInterface::StaticClass()))
                {
                        continue;
                }

                FVector Dir = (TestActor->GetActorLocation() - OwnerLoc).GetSafeNormal();
                float Dot = FVector::DotProduct(Owner->GetActorForwardVector(), Dir);
                if (Dot < BestDot)
                {
                        continue;
                }

                FHitResult Hit;
                bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, OwnerLoc, TestActor->GetActorLocation(), TraceChannel);
                if (bBlocked && Hit.GetActor() != TestActor)
                {
                        continue;
                }

                BestDot = Dot;
                BestActor = TestActor;
        }

        if (CurrentActor != BestActor)
        {
                if (CurrentActor && CurrentActor->GetClass()->ImplementsInterface(UInteractableObjInterface::StaticClass()))
                {
                        IInteractableObjInterface::Execute_UnHighlightActor(CurrentActor);
                }

                CurrentActor = BestActor;

                if (CurrentActor && CurrentActor->GetClass()->ImplementsInterface(UInteractableObjInterface::StaticClass()))
                {
                        IInteractableObjInterface::Execute_HighlightActor(CurrentActor);
                }
        }
}

void UInteractorComponent::TryInteract()
{
        if (CurrentActor && CurrentActor->GetClass()->ImplementsInterface(UInteractableObjInterface::StaticClass()))
        {
                IInteractableObjInterface::Execute_Interact(CurrentActor, GetOwner());
        }
}

