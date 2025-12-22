// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/RCBlockTypes.h"
#include "UObject/NoExportTypes.h"
#include "RCWorldGenerator.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API ARCWorldGenerator : public AActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	class ARCGrid* GetGrid() const { return Grid; };
private:
	void CreateGrid();

	//UPROPERTY(EditDefaultsOnly, Category = "Config")
	//int HeightLimit;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARCGrid> GridClass;
	UPROPERTY()
	class ARCGrid* Grid;
};
