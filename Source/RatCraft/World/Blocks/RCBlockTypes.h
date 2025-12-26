// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RCBlockTypes.generated.h"

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	Air								UMETA(DisplayName = "Air"),
	Grass							UMETA(DisplayName = "Grass"),
	Stone							UMETA(DisplayName = "Stone"),
	Snow							UMETA(DisplayName = "Snow"),
};