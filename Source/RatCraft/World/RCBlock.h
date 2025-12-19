// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "RatCraft/Interactables/RCInteractable.h"
#include "RCBlock.generated.h"

UENUM()
enum class EBlockType : uint8
{
	Air,
	Grass,
	Stone,
	Snow
};

UCLASS()
class RATCRAFT_API ARCBlock : public AActor, public IRCInteractable
{
	GENERATED_BODY()
	
public:	
	/**
	 * 
	 */
	ARCBlock();

	virtual void OnInteract(AActor* InteractActor) override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* StaticMesh;
	
	EBlockType Type = EBlockType::Air;
};
