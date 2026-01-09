// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RatCraft/Inventory/RCInventoryItem.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class URCInventory* PlayerInventory;
	
private:
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* ItemsContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UItemWidget> ItemWidgetClass;
	UPROPERTY()
	TArray<class UItemWidget*> ItemWidgets;
	UPROPERTY()
	uint8 SelectedItemSlot;

	UPROPERTY()
	TMap<uint8 /*Slot*/, FRCInventoryItem> PopulatedItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TMap<TEnumAsByte<EBlockType>, class URC_DataAssetBlockInventory*> InventoryItemsData;

	uint8 InventoryCapacity;
	uint8 MaxStackSize;

	void ItemAdded(const struct FRCInventoryItem& InventoryItem);
	void ItemRemove(const struct FRCInventoryItem& InventoryItem);

	void UpdateSelectedSlot(const uint8 SelectedSlot);

	uint8 GetNextAvailableSlot() const;
};
