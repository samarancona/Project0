// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/Character/CharacterBase.h"
#include "Public/Interaction/InteractableObjInterface.h"
#include "NPC_Character.generated.h"

UCLASS()
class PROJECT0_API ANPC_Character : public ACharacterBase, public IInteractableObjInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC_Character();

	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false; 
};
