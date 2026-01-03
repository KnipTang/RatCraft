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

	void AddChunck(int X, int Y);

	bool SpawnBlock(FVector& Coords);

	class ARCWorldChunck* GetChunkByChunkCoords(const FVector2D& ChunkCoords) const { return AllChunks.FindChecked(ChunkCoords); };
private:
	UPROPERTY()
	const class URCWorldSettings* WorldSettings;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCWorldChunck> ChunksClass;
	UPROPERTY()
	TMap<FVector2D, class ARCWorldChunck*> AllChunks;
};
