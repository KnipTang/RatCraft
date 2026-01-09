// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RatCraft/Inventory/RCInventory.h"
#include "RatCraft/Inventory/RCInventoryItem.h"
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

	void UpdateInventoryItem(const struct FRCInventoryItem& InInventoryItem, const class URC_DataAssetBlockInventory* DataAsset);
	
	bool IsEmpty() const;
	void EmptySlot();
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

	FRCInventoryItem InventoryItem;
};
