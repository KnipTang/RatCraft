// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/Character/RCCharacter.h"
#include "RCPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API ARCPlayerCharacter : public ARCCharacter
{
	GENERATED_BODY()

public:
	ARCPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class UCameraComponent* ViewCam;
	UPROPERTY()
	UCharacterMovementComponent* MovementComp;

private:
	UPROPERTY()
	const class URCWorldSettings* WorldSettings;
	
	UPROPERTY()
	class ARCWorldManager* WorldManager;

	void SetPlayerGridCoords();
	FVector PlayerGridCoords;

	/***************************************************/
	/*						INPUT						/
	/***************************************************/
private:
	void HandleMoveInput(const struct FInputActionValue& InputActionValue);
	void HandleLookInput(const struct FInputActionValue& InputActionValue);
	void HandleMineInput(const struct FInputActionValue& InputActionValue);
	void HandlePlaceInput(const struct FInputActionValue& InputActionValue);
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* GameplayInputMappingContext;
	//Actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MineInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* PlaceInputAction;

	/***************************************************/
	/*					Player Setting					/
	/***************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Player Settings")
	float InteractDistance = 500.f;
	
private:
	void OnPlayerMovement();
	void UpdateWireframe() const;
};