// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/CustomInputComponent.h"
class UInputConfig;

UCustomInputComponent::UCustomInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UCustomInputComponent::AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	/*
	check(InputConfig);
	check(InputSubsystem);
	

	// Add any registered input mappings from the settings!
	if (ULyraSettingsLocal* LocalSettings = ULyraSettingsLocal::Get())
	{	
		// Tell enhanced input about any custom keymappings that the player may have customized
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			if (Pair.Key != NAME_None && Pair.Value.IsValid())
			{
				InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
			}
		}
	}*/
}

void UCustomInputComponent::RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	/*
	check(InputConfig);
	check(InputSubsystem);

	ULyraLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULyraLocalPlayer>();
	check(LocalPlayer);
	
	if (ULyraSettingsLocal* LocalSettings = ULyraSettingsLocal::Get())
	{
		// Remove any registered input contexts
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
		}
		
		// Clear any player mapped keys from enhanced input
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			InputSubsystem->RemovePlayerMappedKey(Pair.Key);
		}
	}
	*/
}

void UCustomInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
