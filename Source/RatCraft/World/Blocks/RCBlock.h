// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RCBlock.generated.h"

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	Dirt							UMETA(DisplayName = "Dirt"),
	Stone							UMETA(DisplayName = "Stone"),
	Snow							UMETA(DisplayName = "Snow"),
	Air								UMETA(DisplayName = "Air"),
};

USTRUCT()
struct FBlockFaceVisibility
{
	GENERATED_BODY()
	
public:
	FBlockFaceVisibility() :
		Top(false), Bottom(false), North(false), East(false), South(false), West(false)
	{
		Faces = {South, North, West, East, Top, Bottom};
	}
	FBlockFaceVisibility(bool InTop, bool InBottom, bool InNorth, bool InEast, bool InSouth, bool InWest) :
		Top(InTop), Bottom(InBottom), North(InNorth), East(InEast), South(InSouth), West(InWest)
	{
		Faces = {South, North, West, East, Top, Bottom};
	}

	
	bool Top = false;
	bool Bottom = false;
	bool North = false;
	bool East = false;
	bool South = false;
	bool West = false;

	TArray<bool> Faces;
};