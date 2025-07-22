// Copyright Epic Games, Inc. All Rights Reserved.

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "Core/Abilities/P0_AbilitySet.h"
#include "Input/P0_InputComponent.h"
#include "Player/P0_PlayerController.h"
#include "Public/Character/P0_Character.h"

AP0_Character::AP0_Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReadyToBindInputs = false;
	
}

void AP0_Character::BeginPlay()
{
	Super::BeginPlay();
}

void AP0_Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

// Called to bind functionality to input
void AP0_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	const AP0_PlayerController* PC = GetController<AP0_PlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMappingContext, 0);
	Subsystem->AddMappingContext(InputMappingContext_Gamepad, 0);

	if (const UP0_InputConfig* inputConfig = InputConfig)
	{
		UP0_InputComponent* BL_InputComponent = CastChecked<UP0_InputComponent>(PlayerInputComponent);
		BL_InputComponent->AddInputMappings(InputConfig, Subsystem);

		TArray<uint32> BindHandles;
		BL_InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
		                                      &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

		BL_InputComponent->BindNativeAction(InputConfig, FGameplayTag::RequestGameplayTag("InputTag.Move"),
		                                    ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/
		                                    true);
		BL_InputComponent->BindNativeAction(InputConfig, FGameplayTag::RequestGameplayTag("InputTag.Look.Mouse"),
		                                    ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse,
		                                    /*bLogIfNotFound=*/
		                                    true);
		BL_InputComponent->BindNativeAction(InputConfig, FGameplayTag::RequestGameplayTag("InputTag.Look.Stick"),
		                                    ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick,
		                                    /*bLogIfNotFound=*/
		                                    true);
	}

	// Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
	BindASCInput();

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
}

void AP0_Character::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent && IsValid(InputComponent))
	{
		const FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(
			FName("/Script/BloodLoop"), FName("EP0_AbilityInputId"));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(
			                                                              FString("Confirm"),
			                                                              FString("Cancel"), AbilityEnumAssetPath,
			                                                              static_cast<int32>(
				                                                              EP0_AbilityInputId::Confirm),
			                                                              static_cast<int32>(
				                                                              EP0_AbilityInputId::Cancel)));

		ASCInputBound = true;
	}
}

void AP0_Character::Input_AbilityInputTagPressed(FGameplayTag InputTag) const
{
	if (AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void AP0_Character::Input_AbilityInputTagReleased(FGameplayTag InputTag) const
{
	if (AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AP0_Character::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, MovementVector.Y);
	AddMovementInput(RightDir, MovementVector.X);
}

void AP0_Character::Input_LookMouse(const FInputActionValue& InputActionValue)
{
}

void AP0_Character::Input_LookStick(const FInputActionValue& InputActionValue)
{
}


