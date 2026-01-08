// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"
#include "RCInventoryItem.generated.h"
/**
 * 
 */
UCLASS()
class RATCRAFT_API URCInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void Init(EBlockType InBlockType, int8 InCount, int8 InInventorySlot)
	{
		BlockType = InBlockType;
		Count = InCount;
		InventorySlot = InInventorySlot;
	}
	
	EBlockType BlockType;
	int8 Count;
	int8 InventorySlot;
};