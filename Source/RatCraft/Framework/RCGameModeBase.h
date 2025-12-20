// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RCGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API ARCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARCGameModeBase();

	virtual void BeginPlay() override;

	class ARCGrid* GetGrid() const { return Grid; };
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCGrid> GridClass;

	UPROPERTY()
	class ARCGrid* Grid;
};
