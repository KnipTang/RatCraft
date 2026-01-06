// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RCWorldManager.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API ARCWorldManager : public AActor
{
	GENERATED_BODY()

public:
	ARCWorldManager();
	
	virtual void BeginPlay() override;

	void EnableChunkLoading(const FVector* PlayerGridCoords);

	void HandleChunckLoading(const FVector* PlayerGridCoords);

	void RenderChunck(const FVector2D& Coords);
	void AddChunck(int X, int Y);

	void Mining(const bool bIsPressed);

	bool SpawnBlock(const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight);
	void UpdateWireframe() const;
	
	void UpdateInteractableChunck(const float InteractDistance, const FVector& ViewCamLocation, const FRotator& ViewCamRotation);
	class ARCWorldChunck* GetChunkByChunkCoords(const FVector2D& ChunkCoords) const { return AllChunks.FindChecked(ChunkCoords); };
private:
	/***************************************************/
	/*					Interacting						/
	/***************************************************/
	void LookAtChunckChanged(class ARCWorldChunck* NewChunck);

	void StartCanPlaceBlockTimer();
	
	UPROPERTY()
	class ARCWorldChunck* CurrentlyLookAtChunck;
	bool bIsLookingAtChunk;
	FVector LookAtBlockNormal;
	FVector LookAtBlockCoords;

	//BLOCK PLACEMENT
	bool bCanPlaceBlock = true;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float BlockPlacedCooldown = 0.1f;
	FTimerHandle BlockPlacedTimerHandle;
	
	bool CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight) const;
	bool IsPlayerObstructing(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, float ColliderSize, float ColliderHeight) const;

	FVector2D GetChunkCoords(const FVector& WorldCoords) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCBlock> WireframeBlockClass;
	UPROPERTY()
	class ARCBlock* WireframeBlock;
	
	UPROPERTY()
	const class URCWorldSettings* WorldSettings;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCWorldChunck> ChunksClass;
	UPROPERTY()
	TMap<FVector2D, class ARCWorldChunck*> AllChunks;
	UPROPERTY()
	TArray<class ARCWorldChunck*> RenderedChunks;

	UPROPERTY(EditDefaultsOnly, Category = "World Management")
	float UpdateWorldRenderCooldown = 1.f;
	FTimerHandle UpdateWorldRenderTimerHandle;
};
