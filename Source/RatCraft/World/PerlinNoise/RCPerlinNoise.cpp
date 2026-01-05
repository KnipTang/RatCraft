// Fill out your copyright notice in the Description page of Project Settings.


#include "RCPerlinNoise.h"
#pragma optimize("", off)
TArray<float> URCPerlinNoise::GenerateHeightMap(int32 Width, int32 Height, float Scale, FVector2D Offset, int Seed)
{
	TArray<float> HeightMap;
	HeightMap.SetNum(Width * Height);

	FRandomStream RandomStream(Seed);
    
	FVector2D SeedOffset(
		RandomStream.FRandRange(0.0f, 10000.0f),
		RandomStream.FRandRange(0.0f, 10000.0f)
	);

	Offset+=SeedOffset;
	
	for (uint8 x = 0; x < Width; x++)
	{
		for (uint8 y = 0; y < Height; y++)
		{
			const float SampleX = (x + Offset.X) * Scale;
			const float SampleY = (y + Offset.Y) * Scale;
            
			const float NoiseValue = FMath::PerlinNoise2D(FVector2D(SampleX, SampleY));
			const float RemappedValue = (NoiseValue + 1.0f) * 0.5f;

			HeightMap[x + y * Width] = RemappedValue;
		}
	}
	
	return HeightMap;
}
