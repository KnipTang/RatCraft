// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RCBlockTypes.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "RatCraft/Interactables/RCInteractable.h"
#include "RCBlock.generated.h"

enum class EBlockFace : uint8
{
	None    UMETA(DisplayName = "None"),
	Top     UMETA(DisplayName = "Top"),
	Bottom  UMETA(DisplayName = "Bottom"),
	North   UMETA(DisplayName = "North"),
	South   UMETA(DisplayName = "South"),
	East    UMETA(DisplayName = "East"),
	West    UMETA(DisplayName = "West")
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

	void Init(const FVector& GridCoords);

	void UpdateMiningProgress();
	virtual void OnInteract() override;
	virtual void EndInteract() override;

	bool IsMining() const { return bIsMining; }

	FVector GetGridCoordinates() const { return GridCoordinates; };
	
private:
	void StartMining();
	void StopMining();
	void OnBlockMined();
	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* StaticMesh;

	//UPROPERTY(EditDefaultsOnly, Category = "Config")
	//const class URCDataAssetBlock * DataAssetBlock;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	EBlockType BlockType;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MineTime;
	
	FVector GridCoordinates;
	
	//Mining
	FTimerHandle MiningTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float MiningUpdateInterval;
	
	float CurrentMinedTime;
	bool bIsMining;
};
