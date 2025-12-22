// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPerlinNoise.h"

TArray<float> URCPerlinNoise::GenerateHeightMap(int32 Width, int32 Height, float Scale, FVector2D Offset)
{
	TArray<float> HeightMap;
	HeightMap.SetNum(Width * Height);
    
	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			const float SampleX = (x + Offset.X) * Scale;
			const float SampleY = (y + Offset.Y) * Scale;
            
			const float NoiseValue = FMath::PerlinNoise2D(FVector2D(SampleX, SampleY));
			HeightMap[x + y * Width] = NoiseValue;
		}
	}
    
	return HeightMap;
}
