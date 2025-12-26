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

	class ARCWorldManager* GetWorldManager() const { return WorldManager; }
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCWorldManager> WorldManagerClass;
	
	UPROPERTY()
	class ARCWorldManager* WorldManager;
};
