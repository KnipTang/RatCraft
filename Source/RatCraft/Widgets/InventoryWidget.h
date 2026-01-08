// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	TMap<uint8 /*BlockTypeID*/, class UItemWidget*> PopulatedItemEntryWidgets;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TMap<EBlockType /*BlockTypeID*/, class URC_DataAssetBlockInventory*> InventoryItemsData;

	void ItemAdded(const class URCInventoryItem* InventoryItem);
	void ItemRemove(const class URCInventoryItem* InventoryItem);

	void UpdateSelectedSlot(const uint8 SelectedSlot);
	UPROPERTY()
	class UItemWidget* SelectedItemWidget;

	class UItemWidget* GetNextAvailableSlot() const;
};
