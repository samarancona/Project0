// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/MainPlayerController.h"

#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Public/Interaction/InteractableObjInterface.h"
#include "InputActionValue.h"
#include "Core/Components/ASC_AbilitySystemComponent.h"
#include "Player/BasePlayerState.h"
#include "Utils/CustomMacros.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AMainPlayerController::AMainPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AMainPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AMainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// CursorTrace();
}

ABasePlayerState* AMainPlayerController::GetCustomPS() const
{
	return CastChecked<ABasePlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UASC_AbilitySystemComponent* AMainPlayerController::GetCustomASC() const
{
	const ABasePlayerState* PS = GetCustomPS();

	return (PS ? CastChecked<UASC_AbilitySystemComponent>(PS->GetAbilitySystemComponent()) : nullptr);
}

void AMainPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UASC_AbilitySystemComponent* ASC = GetCustomASC())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AMainPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

/*void AMainPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AMainPlayerController::OnSetDestinationTriggered()
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

void AMainPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator,
		                                               FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AMainPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AMainPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AMainPlayerController::Move(const FInputActionValue& Value)
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
}*/

void AMainPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = nullptr;

	AActor* HitActor = CursorHit.GetActor();

	if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableObjInterface::StaticClass()))
	{
		ThisActor = HitActor;
		// UE_LOG(LogTemp, Warning, TEXT(">>> Hit actor implements Interactable Interface: %s"), *HitActor->GetName());
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT(">>> Hit actor DOES NOT implement Interactable Interface: %s"), *GetNameSafe(HitActor));
	}

	//  Highlight / Unhighlight
	if (!LastActor)
	{
		if (ThisActor)
		{
			IInteractableObjInterface::Execute_HighlightActor(ThisActor);
		}
	}
	else
	{
		if (!ThisActor)
		{
			IInteractableObjInterface::Execute_UnHighlightActor(LastActor);
		}
		else if (LastActor != ThisActor)
		{
			IInteractableObjInterface::Execute_UnHighlightActor(LastActor);
			IInteractableObjInterface::Execute_HighlightActor(ThisActor);
		}
	}
}
