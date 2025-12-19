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

	class ARCBlock* FindInteractableBlock();

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
};
