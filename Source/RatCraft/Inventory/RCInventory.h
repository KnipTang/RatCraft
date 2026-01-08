// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "RCInventory.generated.h"

UCLASS()
class RATCRAFT_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void Init(uint8 InID, int8 InCount, int8 InInventorySlot)
	{
		BlockTypeID = InID;
		Count = InCount;
		InventorySlot = InInventorySlot;
	}
	
	uint8 BlockTypeID;
	int8 Count;
	int8 InventorySlot;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const class UInventoryItem* /*ItemToGrant*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const class UInventoryItem* /*ItemToRemove*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateSelectedSlotDelegate, const uint8 CurrentlySelectedSlot /*SelectedSlot*/)

UCLASS()
class RATCRAFT_API URCInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	FOnItemAddedDelegate OnItemAdded;
	FOnItemRemovedDelegate OnItemRemoved;

	FOnUpdateSelectedSlotDelegate OnUpdateSelectedSlot;
	
	void AddItem(const uint8 BlockTypeID);
	void RemoveItem();

	void UpdateSelectedSlot(const int8 UpdateValue);

	uint8 GetCurrentlyHoldingBlockTypeID();
	uint8 GetInventoryCapacity() const { return InventoryCapacity; }
	
private:
	UInventoryItem* GetItem(const uint8 BlockTypeID);
	
	UPROPERTY()
	TArray<UInventoryItem*> InventoryStorage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 InventoryCapacity = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	uint8 MaxStackSize = 64;

	uint8 CurrentlySelectedSlot = 0;
	
	uint8 CurrentInventoryCapacity = 0;

	uint8 InvalidCount = 127;

	uint8 AirBlockTypeID;
};
