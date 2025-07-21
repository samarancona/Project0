// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/P0_PlayerController.h"

#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Public/Interaction/InteractableObjInterface.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AP0_PlayerController::AP0_PlayerController(): ShortPressThreshold(0), FXCursor(nullptr), DefaultMappingContext(nullptr),
                                              SetDestinationClickAction(nullptr),
                                              SetDestinationTouchAction(nullptr),
                                              MoveAction(nullptr),
                                              bUseDirectMovement(false),
                                              bMoveToMouseCursor(0), bIsTouch(false)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AP0_PlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AP0_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AP0_PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

       // Set up action bindings
       if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
       {
               if (bUseDirectMovement)
               {
                       if (MoveAction)
                       {
                               EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AP0_PlayerController::Move);
                       }
               }
               else
               {
                       // Setup mouse input events
                       EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AP0_PlayerController::OnInputStarted);
                       EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AP0_PlayerController::OnSetDestinationTriggered);
                       EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AP0_PlayerController::OnSetDestinationReleased);
                       EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AP0_PlayerController::OnSetDestinationReleased);

                       // Setup touch input events
                       EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AP0_PlayerController::OnInputStarted);
                       EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AP0_PlayerController::OnTouchTriggered);
                       EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AP0_PlayerController::OnTouchReleased);
                       EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AP0_PlayerController::OnTouchReleased);
               }
       }
       else
       {
               UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
       }
}

void AP0_PlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AP0_PlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AP0_PlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AP0_PlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AP0_PlayerController::OnTouchReleased()
{
        bIsTouch = false;
        OnSetDestinationReleased();
}

void AP0_PlayerController::Move(const FInputActionValue& Value)
{
       if (APawn* ControlledPawn = GetPawn())
       {
               FVector2D MovementVector = Value.Get<FVector2D>();
               const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

               const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
               const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

               ControlledPawn->AddMovementInput(ForwardDir, MovementVector.Y);
               ControlledPawn->AddMovementInput(RightDir, MovementVector.X);
       }
}

void AP0_PlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/*
	 * Line Trace from cursor. There are several scenarios
	 * A. LastActor is null && ThisActor is null
	 *		- Do Nothing.
	 * B. LastActor is null && ThisActor is Valid
	 *		- Highlight ThisActor
	 * C. LastActor is Valid && ThisActor is null
	 *		- UnHighlight LastActor
	 * D. Both Actors are valid, but LastActor != ThisActor
	 *		- UnHighLight LastActor and Highlight ThisActor
	 * E. Both Actors are Valid and are the same actor
	 *		- Do Nothing
	 */

	if(LastActor == nullptr)
	{
		if(ThisActor != nullptr)
		{
			//Case B
			ThisActor->HighlightActor();
		}
		else
		{
			//Case A - both are null do nothing
		}
	}
	else // LastActor is valid
	{
		if(ThisActor == nullptr)
		{
			//Case C
			LastActor->UnHighlightActor();
		}
		else // BothActors are Valid
		{
			if(LastActor != ThisActor) //Case D
			{
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else //Case E
			{
				//Do Nothing
			}
		}
	}
}
