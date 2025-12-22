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

	void Init(class URCDataAssetBlock* DataAsset, const EBlockType BlockTypeToSpawn, const FVector& GridCoords);

	void UpdateMiningProgress();
	virtual void OnInteract() override;
	virtual void EndInteract() override;

	bool IsMining() const { return bIsMining; }

	FVector GetGridCoordinates() const { return GridCoordinates; };
	
private:
	void StartMining();
	void StopMining();
	void OnBlockMined();

	void CreateMesh(const EBlockType BlockTypeToSpawn);
	void ConfigureBlock(const EBlockType BlockTypeToSpawn);

	FColor GetBlockColorFromBlockType(const EBlockType BlockTypeToSpawn);

	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMesh;
	
	UPROPERTY()
	class URCDataAssetBlock* DataAssetBlock;
	
	FVector GridCoordinates;
	float BlockLength = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	EBlockType BlockType = EBlockType::Air;
	
	//Mining
	FTimerHandle MiningTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float MiningUpdateInterval;
	
	float CurrentMinedTime;
	bool bIsMining;

	bool bIsMinable = false;
};
