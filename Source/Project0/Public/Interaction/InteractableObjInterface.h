// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableObjInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class PROJECT0_API UInteractableObjInterface : public UInterface
{
        GENERATED_BODY()
};

/**
 * 
 */
class PROJECT0_API IInteractableObjInterface
{
        GENERATED_BODY()

        // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
        UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
        void HighlightActor();

        UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
        void UnHighlightActor();

        UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
        void Interact(AActor* Interactor);
};
