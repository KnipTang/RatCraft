// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/Character/RCCharacter.h"
#include "RatCraft/World/Blocks/RCBlock.h"
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

	virtual void Tick(float DeltaSeconds) override;

	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class UCameraComponent* ViewCam;

	/***************************************************/
	/*						INPUT						/
	/***************************************************/
private:
	void HandleMoveInput(const struct FInputActionValue& InputActionValue);
	void HandleLookInput(const struct FInputActionValue& InputActionValue);
	void HandleMineInput(const struct FInputActionValue& InputActionValue);
	void HandlePlaceInput(const struct FInputActionValue& InputActionValue);

	void LookAtBlockChanged(class ARCBlock* NewBlock);
	class ARCBlock* FindInteractableBlock();
	UPROPERTY()
	class ARCBlock* CurrentlyLookedAtBlock;

	void GetBlockFaceFromNormal(const FVector& HitNormal);
	EBlockFace LookAtBlockFace;
	FVector LookAtBlockNormal;

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
};
