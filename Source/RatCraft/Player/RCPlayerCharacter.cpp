// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RatCraft/Abilities/RCAbilitySystemStatics.h"
#include "RatCraft/Framework/RCGameModeBase.h"
#include "RatCraft/World/RCWorldChunck.h"
#include "RatCraft/World/RCWorldManager.h"
#include "RatCraft/World/RCWorldSettings.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"

#pragma optimize("", off)

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

	WorldSettings = URCWorldSettings::GetSettings();
	PlayerGridCoords = GetActorLocation() / WorldSettings->BlockSize;

	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
	{
		if (ARCGameModeBase* RCGameModeBase = Cast<ARCGameModeBase>(GameMode))
		{
			WorldManager = RCGameModeBase->GetWorldManager();
		}
	}

	if (UWorld* World = GetWorld())
		World->GetTimerManager().SetTimer(
		UpdateWorldRenderTimerHandle,
		[this]()
		 {
			WorldManager->HandleChunckLoading(PlayerGridCoords);
		 },
		UpdateWorldRenderCooldown,
		true
	);
}

void ARCPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ARCWorldChunck* LookAtChunck = FindInteractableChunck(); LookAtChunck != CurrentlyLookAtChunck)
	{
		LookAtChunckChanged(LookAtChunck);
	}

	//Player is moving
	if (MovementComp->Velocity != FVector::ZeroVector)
	{
		PlayerGridCoords = GetActorLocation() / WorldSettings->BlockSize;
		UpdateWireframe();
	}
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

	UpdateWireframe();
}

void ARCPlayerCharacter::HandleMineInput(const struct FInputActionValue& InputActionValue)
{
	if (!CurrentlyLookAtChunck)
		return;

	const bool bPressed = InputActionValue.Get<bool>();
	if (bPressed)
	{
		if (!CurrentlyLookAtChunck->IsMining())
			CurrentlyLookAtChunck->OnInteract();
	}
	else //Released
	{
		CurrentlyLookAtChunck->EndInteract();
	}
}

void ARCPlayerCharacter::HandlePlaceInput(const struct FInputActionValue& InputActionValue)
{
	const bool bPressed = InputActionValue.Get<bool>();
	
	if (!bCanPlaceBlock || !CurrentlyLookAtChunck || !WorldManager)
		return;
	
	if (bPressed)
	{
		FVector GridCoordsNewBlock = LookAtBlockCoords + LookAtBlockNormal;
		
		bool bSucceeded = WorldManager->SpawnBlock(GridCoordsNewBlock, PlayerGridCoords, GetCapsuleComponent()->GetScaledCapsuleRadius(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
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

		FindInteractableChunck();
		UpdateWireframe();
	}
}

void ARCPlayerCharacter::LookAtChunckChanged(class ARCWorldChunck* NewChunck)
{
	if (CurrentlyLookAtChunck)
		CurrentlyLookAtChunck->EndInteract();
	
	CurrentlyLookAtChunck = NewChunck;
}

void ARCPlayerCharacter::UpdateWireframe()
{
	if (!WorldManager)
		return;
	
	WorldManager->DisplayWireframe(LookAtBlockCoords, LookAtBlockNormal, bIsLookingAtChunk);
}

class ARCWorldChunck* ARCPlayerCharacter::FindInteractableChunck()
{
	const FHitResult HitResult = URCAbilitySystemStatics::GetHitscanTarget(
		GetWorld(),
		ViewCam->GetComponentLocation(),
		ViewCam->GetComponentRotation(),
		ECC_WorldStatic,
		InteractDistance
		);
	
	if (ARCWorldChunck* InteractedChunck = Cast<ARCWorldChunck>(HitResult.GetActor()))
	{
		bIsLookingAtChunk = true;
		
		LookAtBlockNormal = HitResult.ImpactNormal;
		LookAtBlockCoords = HitResult.Location / WorldSettings->BlockSize -
			FVector(
			FMath::Clamp(LookAtBlockNormal.X, 0, 1),
			FMath::Clamp(LookAtBlockNormal.Y, 0, 1),
			FMath::Clamp(LookAtBlockNormal.Z, 0, 1));
		
		constexpr float SnapEpsilon = 0.0001f;
		LookAtBlockCoords = FVector(
			FMath::Floor(LookAtBlockCoords.X + SnapEpsilon),
			FMath::Floor(LookAtBlockCoords.Y + SnapEpsilon),
			FMath::Floor(LookAtBlockCoords.Z + SnapEpsilon)
		);
		
		InteractedChunck->SetCurrentlyLookAtBlock(LookAtBlockCoords);
		
		return InteractedChunck;
	}

	bIsLookingAtChunk = false;
	return nullptr;
}
