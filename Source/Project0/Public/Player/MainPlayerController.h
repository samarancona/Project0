// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "Core/Components/ASC_AbilitySystemComponent.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class IInteractableObjInterface;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class PROJECT0_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();


	/*
	/** MappingContext #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	
	/** Jump Input Action #3#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	/** Movement input action used when WASD/controller mode is enabled #3#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** If true, movement will be handled via WASD/controller instead of click #3#
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	bool bUseDirectMovement = false;

	
	/** Time Threshold to know if it was a short press #3#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking #3#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

protected:
	/** Input handlers for SetDestination action. #3#
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	void Move(const FInputActionValue& Value);
private:
FVector CachedDestination;
	/** True if the controlled character should navigate to the mouse cursor. #2#
	uint32 bMoveToMouseCursor : 1;
bool bIsTouch; // Is it a touch device
float FollowTime; // For how long it has been pressed
*/


	
protected:
	

	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	ABasePlayerState* GetCustomPS() const;
	UASC_AbilitySystemComponent* GetCustomASC() const;

private:
	void CursorTrace();

	AActor* LastActor = nullptr;
	AActor* ThisActor = nullptr;
};
