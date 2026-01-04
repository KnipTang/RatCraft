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

	void HandleChunckLoading(const FVector& PlayerCoords);

	void RenderChunck(const FVector2D& Coords);
	void AddChunck(int X, int Y);

	bool SpawnBlock(FVector& Coords, const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight);

	class ARCWorldChunck* GetChunkByChunkCoords(const FVector2D& ChunkCoords) const { return AllChunks.FindChecked(ChunkCoords); };
private:
	FVector2D GetChunkCoords(const FVector& WorldCoords);
	
	UPROPERTY()
	const class URCWorldSettings* WorldSettings;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCWorldChunck> ChunksClass;
	UPROPERTY()
	TMap<FVector2D, class ARCWorldChunck*> AllChunks;
	UPROPERTY()
	TArray<class ARCWorldChunck*> RenderedChunks;
};
