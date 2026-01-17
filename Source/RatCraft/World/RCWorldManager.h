// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/RCBlockStatics.h"
#include "GameFramework/Actor.h"
#include "RCWorldManager.generated.h"

	//DECLARE_STATS_GROUP(TEXT("ARCGame"), STATGROUP_ARCGame, STATCAT_Advanced)
//
	//DECLARE_CYCLE_STAT(TEXT("HandleChunkLoading"), STAT_HandleChunkLoading, STATGROUP_ARCGame);
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

	void Mining(const bool bIsPressed) const;
	void OnBlockMined(const EBlockType BlockType) const;

	bool SpawnBlock(const EBlockType BlockType,const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight);
	
	void UpdateWireframe() const;
	
	void UpdateInteractableChunk(const float InteractDistance, const FVector& ViewCamLocation, const FRotator& ViewCamRotation);

	void SetPlayerInventory(class URCInventory* InPlayerInventory);
	
	class ARCWorldChunk* GetChunkAtWorldCoords(const int X, const int Y);
	class URCDataAssetBlock* GetDataAssetBlockFromType(EBlockType BlockType) const;
	UMaterialInterface* GetMaterialFromTypeID(const uint8 BlockTypeID);
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	ARCWorldChunk* AddChunk(int X, int Y);
	
	void HandleChunkLoading(const FVector* PlayerGridCoords);
	bool DidChunkChange(const FVector2D& ChunkCoords);

	FVector2D GetChunkCoordsFromWorldCoords(int X, int Y) const;

	UPROPERTY()
	const class URCWorldSettings* WorldSettings;

	UPROPERTY()
	class URCInventory* PlayerInventory;
	
	UPROPERTY(EditDefaultsOnly, Category = "Blocks")
	TMap<TEnumAsByte<EBlockType>, class URCDataAssetBlock*> BlockDataAsset;
	
	UPROPERTY()
	TMap<uint8 /*BlockTypeID*/, UMaterialInterface*> BlockTypeMaterials;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCWorldChunk> ChunksClass;
	UPROPERTY()
	TMap<FVector2D, class ARCWorldChunk*> AllChunks;
	UPROPERTY()
	TArray<FVector2D> RenderedChunks;

	FActorSpawnParameters SpawnParams;
	
	UPROPERTY(EditDefaultsOnly, Category = "World Management")
	float UpdateWorldRenderCooldown = 1.f;
	FTimerHandle UpdateWorldRenderTimerHandle;
	
	/***************************************************/
	/*					Interacting						/
	/***************************************************/
	UPROPERTY()
	class ARCWorldChunk* CurrentlyLookAtChunk;
	UPROPERTY()
	FVector2D CurrentlyStandOnChunkCoords;
	bool bIsLookingAtChunk;
	FVector LookAtBlockNormal;
	FVector LookAtBlockCoords;
	
	void LookAtChunkChanged(class ARCWorldChunk* NewChunk);

	//BLOCK PLACEMENT
	bool bCanPlaceBlock = true;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float BlockPlacedCooldown = 0.1f;
	FTimerHandle BlockPlacedTimerHandle;
	
	void StartCanPlaceBlockTimer();
	bool CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight) const;
	bool IsPlayerObstructing(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, float ColliderSize, float ColliderHeight) const;
	
	//WIREFRAME
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCBlock> WireframeBlockClass;
	UPROPERTY()
	class ARCBlock* WireframeBlock;
};
