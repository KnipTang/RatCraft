// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/RCBlockStatics.h"
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

	void EnableChunkLoading(const FVector* PlayerGridCoords);

	void Mining(const bool bIsPressed);

	bool SpawnBlock(const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight);
	
	void UpdateWireframe() const;
	
	void UpdateInteractableChunck(const float InteractDistance, const FVector& ViewCamLocation, const FRotator& ViewCamRotation);

	class ARCWorldChunck* GetChunkAtWorldCoords(const int X, const int Y);

protected:
	virtual void BeginPlay() override;
	
private:
	void RenderChunck(const FVector2D& Coords);
	void AddChunck(int X, int Y);
	
	void HandleChunckLoading(const FVector* PlayerGridCoords);

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
	
	FVector2D GetChunkCoordsFromWorldCoords(const int X, const int Y) const;

	//WIREFRAME
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCBlock> WireframeBlockClass;
	UPROPERTY()
	class ARCBlock* WireframeBlock;
};
