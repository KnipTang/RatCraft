// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RatCraft/Inventory/RCInventory.h"
#include "ItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void UpdateInventoryItem(const class URCInventoryItem* InInventoryItem, const class URC_DataAssetBlockInventory* DataAsset);
	
	bool IsEmpty() const;
	void EmptySlot();
	void SetSlotNumber(uint8 NewSlotNumber);
	void SetIcon(UTexture2D* IconTexture) const;
	void SetCount(uint8 Count) const;

	void ChangeSelected(bool bSelected) const;
private:
	UPROPERTY(meta = (BindWidget))
	class UBorder* Background;
	UPROPERTY(meta=(bindWidget))
	class UImage* ItemIcon;
	UPROPERTY(meta=(bindWidget))
    class UTextBlock* ItemCount;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FLinearColor ColorBackgroundUnselected;
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FLinearColor ColorBackgroundSelected;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	class UTexture2D* EmptyTexture;

	UPROPERTY()
	const class URCInventoryItem* InventoryItem;
	
	int SlotNumber;
};
