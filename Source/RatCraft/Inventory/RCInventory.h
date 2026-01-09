// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"
#include "RCInventory.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const struct FRCInventoryItem& /*ItemToGrant*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const struct FRCInventoryItem& /*ItemToRemove*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateSelectedSlotDelegate, const uint8 CurrentlySelectedSlot /*SelectedSlot*/)

UCLASS()
class RATCRAFT_API URCInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	void Init();
	
	FOnItemAddedDelegate OnItemAdded;
	FOnItemRemovedDelegate OnItemRemoved;

	FOnUpdateSelectedSlotDelegate OnUpdateSelectedSlot;
	
	void AddItem(const EBlockType BlockType);
	void RemoveItem();

	void UpdateSelectedSlot(const int8 UpdateValue);

	EBlockType GetCurrentlyHoldingBlockType();
	uint8 GetInventoryCapacity() const { return InventoryCapacity; }
	
private:
	struct FRCInventoryItem* GetItem(const EBlockType BlockType);
	
	UPROPERTY()
	TArray<struct FRCInventoryItem> InventoryStorage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 InventoryCapacity = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 MaxStackSize = 64;

	uint8 CurrentlySelectedSlot = 0;

	uint8 InvalidCount = 127;
};
