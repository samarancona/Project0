// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "AbilitySystemInterface.h"
#include "AMainCharacter.generated.h"

UCLASS(Blueprintable)
class PROJECT0_API AMainCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AMainCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called from both SetupPlayerInputComponent and OnRep_PlayerState because of a potential race condition where the PlayerController might
	// call ClientRestart which calls SetupPlayerInputComponent before the PlayerState is repped to the client so the PlayerState would be null in SetupPlayerInputComponent.
	// Conversely, the PlayerState might be repped before the PlayerController calls ClientRestart so the Actor's InputComponent would be null in OnRep_PlayerState.
	void BindASCInput();
	bool ASCInputBound = false;

	/** Trace channel to use for mouse aim */
	UPROPERTY(EditAnywhere, Category="Input")
	TEnumAsByte<ETraceTypeQuery> MouseAimTraceChannel;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	void DoMove(float AxisX, float AxisY);

	/** Handles aim inputs from both input actions and touch interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoAim(float AxisX, float AxisY);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Possessed by controller initialization */
	virtual void NotifyControllerChanged() override;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	/** Pointer to the player controller assigned to this character */
	TObjectPtr<APlayerController> PlayerController;

	/** If true, the player is using mouse aim */
	bool bUsingMouse = false;

	/** Speed to blend between our current rotation and the target aim rotation when stick aiming */
	UPROPERTY(EditAnywhere, Category="Aim", meta=(ClampMin = 0, ClampMax = 100))
	float AimRotationInterpSpeed = 10.0f;

	/** Aim Yaw Angle in degrees */
	float AimAngle = 0.0f;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputConfig> InputConfig;

	// Input configuration Mapping Context
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	const UInputMappingContext* InputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	const UInputMappingContext* InputMappingContext_Gamepad;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;


	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;

	/** Last held move input */
	FVector2D LastMoveInput;
};
