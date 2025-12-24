// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RatCraft/Abilities/RCAbilitySystemStatics.h"
#include "RatCraft/Framework/RCGameModeBase.h"
#include "RatCraft/Interactables/RCInteractable.h"
#include "RatCraft/World/RCWorldManager.h"
#include "RatCraft/World/Blocks/RCBlock.h"
#include "RatCraft/World/Grid/RCGrid.h"

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

	MovementComp = GetCharacterMovement();
	if (!MovementComp)
		return;
	
	MovementComp->bOrientRotationToMovement = false;
	MovementComp->bUseControllerDesiredRotation = false;
	MovementComp->RotationRate = FRotator( 0.0f,720.0f,0.0f );
}

void ARCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
	{
		if (ARCGameModeBase* RCGameModeBase = Cast<ARCGameModeBase>(GameMode))
		{
			GridRef = RCGameModeBase->GetGrid();
		}
	}
}

void ARCPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ARCBlock* LookAtBlock = FindInteractableBlock(); LookAtBlock != CurrentlyLookedAtBlock)
	{
		LookAtBlockChanged(LookAtBlock);
	}

	if (MovementComp->Velocity != FVector::ZeroVector)
		PlayerGridCoords = GridRef->GetGridCoordsFromWorldPosition(GetActorLocation());
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
		EnhancedInputComp->BindAction(PlaceInputAction, ETriggerEvent::Triggered, this, &ARCPlayerCharacter::HandlePlaceInput);
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

	if (!CurrentlyLookedAtBlock)
		return;
	
	if (bPressed)
	{
		if (!CurrentlyLookedAtBlock->IsMining())
			CurrentlyLookedAtBlock->OnInteract();
	}
	else //Released
	{
		CurrentlyLookedAtBlock->EndInteract();
	}
}

void ARCPlayerCharacter::HandlePlaceInput(const struct FInputActionValue& InputActionValue)
{
	const bool bPressed = InputActionValue.Get<bool>();

	if (!bCanPlaceBlock || !CurrentlyLookedAtBlock)
		return;
	
	if (bPressed)
	{
		const FVector GridCoordsNewBlock = CurrentlyLookedAtBlock->GetGridCoordinates() + LookAtBlockNormal;
		
		if (GridRef->CanSpawnBlockAtGridCoords(GridCoordsNewBlock, PlayerGridCoords, GetCapsuleComponent()->GetScaledCapsuleRadius() / 2.f))
		{
			const FBlockFaceVisibility BlockFaceVisibility{true, true, true, true, true, true};
			bool bSucceeded = GridRef->SpawnBlock(EBlockType::Grass, GridCoordsNewBlock, BlockFaceVisibility);
			if (!bSucceeded)
				return;

			bCanPlaceBlock = false;

			if (UWorld* World = GetWorld())
				World->GetTimerManager().SetTimer(
				BlockPlacedTimerHandle,
				[this]()
				 {
					 bCanPlaceBlock = true;
				 },
				BlockPlacedCooldown,
				false
			);
		}
	}
}

void ARCPlayerCharacter::LookAtBlockChanged(class ARCBlock* NewBlock)
{
	if (CurrentlyLookedAtBlock && CurrentlyLookedAtBlock->IsMining())
		CurrentlyLookedAtBlock->EndInteract();
	
	CurrentlyLookedAtBlock = NewBlock;
}

class ARCBlock* ARCPlayerCharacter::FindInteractableBlock()
{
	const FHitResult HitResult = URCAbilitySystemStatics::GetHitscanTarget(
		GetWorld(),
		ViewCam->GetComponentLocation(),
		ViewCam->GetComponentRotation(),
		ECC_WorldStatic,
		400
		);

	AActor* PossibleInteractableActor = HitResult.GetActor();
	if (ARCBlock* InteractableBlock = Cast<ARCBlock>(PossibleInteractableActor))
	{
		LookAtBlockNormal = HitResult.ImpactNormal;
		
		return InteractableBlock;
	}
	return nullptr;
}
