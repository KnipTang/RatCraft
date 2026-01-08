// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RCInventoryItem.generated.h"

USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()
public:
	FInventoryItemHandle();
	static FInventoryItemHandle CreateHandle();
	static FInventoryItemHandle InvalidHandle();

	bool IsValid() const;
	uint32 GetHandleID() const { return HandleID; };
private:
	explicit FInventoryItemHandle(uint32 ID);

	UPROPERTY()
	uint32 HandleID;

	static uint32 GenerateNextID();
	static uint32 GetInvalidID();
};

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs);
uint32 GetTypeHash(const FInventoryItemHandle& Key);
/**
 * 
 */
UCLASS()
class RATCRAFT_API URCInventoryItem : public UObject
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