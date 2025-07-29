// Copyright Epic Games, Inc. All Rights Reserved.
#include "Public/Character/AMainCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "Core/Abilities/AbilitySet.h"
#include "Input/CustomInputComponent.h"
#include "Interaction/InteractorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/MainPlayerController.h"

AMainCharacter::AMainCharacter()
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

	InteractorComponent = CreateDefaultSubobject<UInteractorComponent>(TEXT("InteractorComponent"));
	

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReadyToBindInputs = false;
	
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMainCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// get the current rotation
	const FRotator OldRotation = GetActorRotation();

	// are we aiming with the mouse?
	if (bUsingMouse)
	{
		if (PlayerController)
		{
			// get the cursor world location
			FHitResult OutHit; 
			PlayerController->GetHitResultUnderCursorByChannel(MouseAimTraceChannel, true, OutHit);

			// find the aim rotation 
			const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OutHit.Location);

			// save the aim angle
			AimAngle = AimRot.Yaw;

			

			// update the yaw, reuse the pitch and roll
			SetActorRotation(FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll));

		}

	} else {

		// use quaternion interpolation to blend between our current rotation
		// and the desired aim rotation using the shortest path
		const FRotator TargetRot = FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll);

		SetActorRotation(TargetRot);
	}
}

void AMainCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// set the player controller reference
	PlayerController = Cast<APlayerController>(GetController());
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	const AMainPlayerController* PC = GetController<AMainPlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMappingContext, 0);
	Subsystem->AddMappingContext(InputMappingContext_Gamepad, 0);

	if (const UInputConfig* inputConfig = InputConfig)
	{
		UCustomInputComponent* P0_InputComponent = CastChecked<UCustomInputComponent>(PlayerInputComponent);
		P0_InputComponent->AddInputMappings(inputConfig, Subsystem);

		TArray<uint32> BindHandles;
		P0_InputComponent->BindAbilityActions(inputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
		                                      &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

		P0_InputComponent->BindNativeAction(inputConfig, FGameplayTag::RequestGameplayTag("InputTag.Move"),
		                                    ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/
		                                    true);
		P0_InputComponent->BindNativeAction(inputConfig, FGameplayTag::RequestGameplayTag("InputTag.Look.Mouse"),
		                                    ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse,
		                                    /*bLogIfNotFound=*/
		                                    true);
		P0_InputComponent->BindNativeAction(inputConfig, FGameplayTag::RequestGameplayTag("InputTag.Look.Stick"),
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


void AMainCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent && IsValid(InputComponent))
	{
		const FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(
			FName("/Script/Project0"), FName("EAbilityInputId"));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(
			                                                              FString("Confirm"),
			                                                              FString("Cancel"), AbilityEnumAssetPath,
			                                                              static_cast<int32>(
				                                                              EAbilityInputId::Confirm),
			                                                              static_cast<int32>(
				                                                              EAbilityInputId::Cancel)));

		ASCInputBound = true;
	}
}

void AMainCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag) 
{
	if (AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void AMainCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AMainCharacter::Input_Move(const FInputActionValue& InputActionValue)
{

	// save the input vector
	FVector2D InputVector = InputActionValue.Get<FVector2D>();

	// route the input
	DoMove(InputVector.X, InputVector.Y);
	
	/*
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, MovementVector.Y);
	AddMovementInput(RightDir, MovementVector.X);*/
}

void AMainCharacter::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	// raise the mouse controls flag
	bUsingMouse = true;

	// show the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
	}
}

void AMainCharacter::Input_LookStick(const FInputActionValue& InputActionValue)
{
	// get the input vector
	FVector2D InputVector = InputActionValue.Get<FVector2D>();

	// route the input
	DoAim(InputVector.X, InputVector.Y);
}

void AMainCharacter::DoMove(float AxisX, float AxisY)
{
	// save the input
	LastMoveInput.X = AxisX;
	LastMoveInput.Y = AxisY;

	// calculate the forward component of the input
	FRotator FlatRot = GetControlRotation();
	FlatRot.Pitch = 0.0f;

	// apply the forward input
	AddMovementInput(FlatRot.RotateVector(FVector::ForwardVector), AxisX);

	// apply the right input
	AddMovementInput(FlatRot.RotateVector(FVector::RightVector), AxisY);
}


void AMainCharacter::DoAim(float AxisX, float AxisY)
{
	// calculate the aim angle from the inputs
	AimAngle = FMath::RadiansToDegrees(FMath::Atan2(AxisY, -AxisX));

	// lower the mouse controls flag
	bUsingMouse = false;

	// hide the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
	}
}


