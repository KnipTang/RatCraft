// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "RCInventory.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const class URCInventoryItem* /*ItemToGrant*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const class URCInventoryItem* /*ItemToRemove*/)
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
	
	void AddItem(const uint8 BlockTypeID);
	void RemoveItem();

	void UpdateSelectedSlot(const int8 UpdateValue);

	uint8 GetCurrentlyHoldingBlockTypeID();
	uint8 GetInventoryCapacity() const { return InventoryCapacity; }
	
private:
	class URCInventoryItem* GetItem(const uint8 BlockTypeID);
	
	UPROPERTY()
	TArray<class URCInventoryItem*> InventoryStorage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 InventoryCapacity = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 MaxStackSize = 64;

	uint8 CurrentlySelectedSlot = 0;

	uint8 InvalidCount = 127;

	uint8 AirBlockTypeID;
};
