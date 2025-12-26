// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/RCBlockTypes.h"
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
	UPROPERTY()
	TArray<class ARCWorldChunck*> Chunks;
};
