// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RatCraft/Framework/RCGameModeBase.h"
#include "RatCraft/World/RCWorldManager.h"
#include "RatCraft/World/RCWorldSettings.h"

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
	SetPlayerGridCoords();

	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
	{
		if (const ARCGameModeBase* RCGameModeBase = Cast<ARCGameModeBase>(GameMode))
		{
			WorldManager = RCGameModeBase->GetWorldManager();
		}
	}

	if (WorldManager)
	{
		WorldManager->EnableChunkLoading(&PlayerGridCoords);
	}
}

void ARCPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WorldManager->UpdateInteractableChunk(InteractDistance, ViewCam->GetComponentLocation(), ViewCam->GetComponentRotation());

	//Player is moving
	if (MovementComp->Velocity != FVector::ZeroVector)
	{
		OnPlayerMovement();
	}
}

void ARCPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (const APlayerController* OwningPlayerController = GetController<APlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
}

void ARCPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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

void ARCPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	if (!Controller)
		return;
	
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	InputVal.Normalize();

	const FVector LookRightDir = ViewCam->GetRightVector();
	const FVector MoveForwardDir = FVector::CrossProduct(LookRightDir, FVector::UpVector);
	AddMovementInput(MoveForwardDir * InputVal.Y + LookRightDir * InputVal.X);
}

void ARCPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	const FVector2D InputVal = InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(-InputVal.Y);
	AddControllerYawInput(InputVal.X);

	UpdateWireframe();
}

void ARCPlayerCharacter::HandleMineInput(const FInputActionValue& InputActionValue)
{
	if (!WorldManager)
		return;
	
	WorldManager->Mining(InputActionValue.Get<bool>());
}

void ARCPlayerCharacter::HandlePlaceInput(const FInputActionValue& InputActionValue)
{
	if (!WorldManager)
		return;
	
	if (InputActionValue.Get<bool>())
	{
		bool bSucceeded = WorldManager->SpawnBlock(PlayerGridCoords, GetCapsuleComponent()->GetScaledCapsuleRadius(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

		if (bSucceeded)
			WorldManager->UpdateInteractableChunk(InteractDistance, ViewCam->GetComponentLocation(), ViewCam->GetComponentRotation());
	}
}

void ARCPlayerCharacter::OnPlayerMovement()
{
	SetPlayerGridCoords();
	UpdateWireframe();
}

void ARCPlayerCharacter::SetPlayerGridCoords()
{
	PlayerGridCoords = GetActorLocation() / WorldSettings->BlockSize;
}

void ARCPlayerCharacter::UpdateWireframe() const
{
	if (!WorldManager)
		return;
	
	WorldManager->UpdateWireframe();
}