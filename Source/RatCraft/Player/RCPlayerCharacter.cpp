// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RatCraft/Abilities/RCAbilitySystemStatics.h"
#include "RatCraft/Interactables/RCInteractable.h"
#include "RatCraft/World/RCBlock.h"

ARCPlayerCharacter::ARCPlayerCharacter()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Skeletal mesh component of player not found"));
		return;
	}
	
	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(RootComponent);
	ViewCam->SetRelativeLocation(FVector(0, 0, 64.f));
	ViewCam->bUsePawnControlRotation = true;
    
	// Movement settings
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
    
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator( 0.0f,720.0f,0.0f );
}

void ARCPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* OwningPlayerController = GetController<APlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
}

void ARCPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleMoveInput);
		EnhancedInputComp->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleLookInput);
		EnhancedInputComp->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::Jump);
		EnhancedInputComp->BindAction(MineInputAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandleMineInput);
	}
}

void ARCPlayerCharacter::HandleMoveInput(const struct FInputActionValue& InputActionValue)
{
	if (!Controller)
		return;
	
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	InputVal.Normalize();

	const FVector LookRightDir = ViewCam->GetRightVector();
	const FVector MoveForwardDir = FVector::CrossProduct(LookRightDir, FVector::UpVector);
	AddMovementInput(MoveForwardDir * InputVal.Y + LookRightDir * InputVal.X);
}

void ARCPlayerCharacter::HandleLookInput(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputVal = InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(-InputVal.Y);
	AddControllerYawInput(InputVal.X);
}

void ARCPlayerCharacter::HandleMineInput(const struct FInputActionValue& InputActionValue)
{
	const bool bPressed = InputActionValue.Get<bool>();

	if (bPressed)
	{
		ARCBlock* block = FindInteractableBlock();
		if (block)
		{
			FString blockName = block->GetName();
			UE_LOG(LogTemp, Display, TEXT("Block found: %s"), *blockName);
    
			// Alternative if you want to use the actor's label (more user-friendly)
			// FString blockLabel = block->GetActorLabel();
			// UE_LOG(LogTemp, Display, TEXT("Block found: %s"), *blockLabel);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No interactable block found"));
		}
	}
	else //Released
	{
		
	}
}

class ARCBlock* ARCPlayerCharacter::FindInteractableBlock()
{
	const FHitResult HitResult = URCAbilitySystemStatics::GetHitscanTarget(
		GetWorld(),
		ViewCam->GetComponentLocation(),
		ViewCam->GetComponentRotation(),
		ECC_WorldStatic,
		1000
		);

	AActor* PossibleInteractableActor = HitResult.GetActor();
	if (ARCBlock* InteractableBlock = Cast<ARCBlock>(PossibleInteractableActor))
	{
		return InteractableBlock;
	}
	return nullptr;
}
