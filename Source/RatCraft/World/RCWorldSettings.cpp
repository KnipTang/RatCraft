// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldSettings.h"

URCWorldSettings::URCWorldSettings()
{
	HalfBlockSize = {BlockSize / 2.f};
	WorldChunckSize = {ChunckSize * BlockSize};
	
	FaceNormals = {
		FVector(0, -1, 0),  // South
		FVector(0, 1, 0),   // North
		FVector(-1, 0, 0),  // West
		FVector(1, 0, 0),   // East
		FVector(0, 0, 1),   // Top
		FVector(0, 0, -1)   // Bottom
	};

	CubeVertices = {
		FVector(-HalfBlockSize, -HalfBlockSize, -HalfBlockSize), // Front-bottom-left
		FVector(HalfBlockSize, -HalfBlockSize, -HalfBlockSize),  // Front-bottom-right
		FVector(HalfBlockSize, -HalfBlockSize, HalfBlockSize),   // Front-top-right
		FVector(-HalfBlockSize, -HalfBlockSize, HalfBlockSize),  // Front-top-left
		FVector(-HalfBlockSize, HalfBlockSize, -HalfBlockSize),  // Back-bottom-left
		FVector(HalfBlockSize, HalfBlockSize, -HalfBlockSize),   // Back-bottom-right
		FVector(HalfBlockSize, HalfBlockSize, HalfBlockSize),    // Back-top-right
		FVector(-HalfBlockSize, HalfBlockSize, HalfBlockSize)    // Back-top-left
	};
}
