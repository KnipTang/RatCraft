// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RCPerlinNoise.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API URCPerlinNoise : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static TArray<float> GenerateHeightMap(int32 Width, int32 Height, float Scale, FVector2D Offset, int Seed);
};
