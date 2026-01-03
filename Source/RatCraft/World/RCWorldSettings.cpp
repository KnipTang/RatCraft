// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldSettings.h"

URCWorldSettings::URCWorldSettings()
{
	FaceNormals = {
		FVector(0, -1, 0),  // South
		FVector(0, 1, 0),   // North
		FVector(-1, 0, 0),  // West
		FVector(1, 0, 0),   // East
		FVector(0, 0, 1),   // Top
		FVector(0, 0, -1)   // Bottom
	};

	CubeVertices = {
		FVector(-GetHalfBlockSize(), -GetHalfBlockSize(), -GetHalfBlockSize()), // Front-bottom-left
		FVector(GetHalfBlockSize(), -GetHalfBlockSize(), -GetHalfBlockSize()),  // Front-bottom-right
		FVector(GetHalfBlockSize(), -GetHalfBlockSize(), GetHalfBlockSize()),   // Front-top-right
		FVector(-GetHalfBlockSize(), -GetHalfBlockSize(), GetHalfBlockSize()),  // Front-top-left
		FVector(-GetHalfBlockSize(), GetHalfBlockSize(), -GetHalfBlockSize()),  // Back-bottom-left
		FVector(GetHalfBlockSize(), GetHalfBlockSize(), -GetHalfBlockSize()),   // Back-bottom-right
		FVector(GetHalfBlockSize(), GetHalfBlockSize(), GetHalfBlockSize()),    // Back-top-right
		FVector(-GetHalfBlockSize(), GetHalfBlockSize(), GetHalfBlockSize())    // Back-top-left
	};
}
