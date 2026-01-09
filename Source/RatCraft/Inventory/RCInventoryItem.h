// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"
#include "RCInventoryItem.generated.h"
/**
 * 
 */
USTRUCT()
struct RATCRAFT_API FRCInventoryItem
{
	GENERATED_BODY()

public:
	FRCInventoryItem() :
		BlockType(EBlockType::Air),
		Count(0)
	{}
	FRCInventoryItem(EBlockType InBlockType, uint8 InCount) :
		BlockType(InBlockType),
		Count(InCount)
	{}
	
	EBlockType BlockType;
	uint8 Count;
};