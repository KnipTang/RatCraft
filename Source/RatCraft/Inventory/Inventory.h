// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Map.h"
#include "Inventory.generated.h"

USTRUCT()
struct FInventoryItem
{
	GENERATED_BODY()
    
	UPROPERTY()
	uint8 BlockTypeID;
    
	UPROPERTY()
	int8 Count;
    
	FInventoryItem() : BlockTypeID(0), Count(0) {}
	FInventoryItem(uint8 InID, int8 InCount) : BlockTypeID(InID), Count(InCount) {}
};

UCLASS()
class RATCRAFT_API UInventory : public UObject
{
	GENERATED_BODY()

public:
	UInventory();
	
	void AddItem(const uint8 BlockTypeID);
	void RemoveItem();

	uint8 GetCurrentlyHoldingBlockTypeID();
private:
	int8& GetItemCount(const uint8 BlockTypeID);
	
	UPROPERTY()
	TArray<FInventoryItem> InventoryStorage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 InventoryCapacity = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 MaxStackSize = 64;

	uint8 CurrentlySelectedSlot = 0;
	
	uint8 CurrentInventoryCapacity = 0;

	int8 InvalidValue = 127;

	uint8 AirBlockTypeID;
};
