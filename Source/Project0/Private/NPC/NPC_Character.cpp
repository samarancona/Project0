// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/NPC/NPC_Character.h"

#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ANPC_Character::ANPC_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ANPC_Character::HighlightActor_Implementation()
{
	bHighlighted = true;
	UKismetSystemLibrary::PrintString(this,TEXT("HighlightActor"));
}

void ANPC_Character::UnHighlightActor_Implementation()
{
        bHighlighted = false;
        UKismetSystemLibrary::PrintString(this,TEXT("UnHighlightActor"));

}

void ANPC_Character::Interact_Implementation(AActor* Interactor)
{
        UKismetSystemLibrary::PrintString(this, TEXT("Interact with NPC"));
}

