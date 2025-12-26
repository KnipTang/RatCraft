// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldManager.h"
#include "RCWorldChunck.h"
#include "RCWorldSettings.h"

ARCWorldManager::ARCWorldManager()
{
	WorldSettings = URCWorldSettings::GetSettings();
}

void ARCWorldManager::BeginPlay()
{
	Super::BeginPlay();

	AddChunck(0, 0);
	AddChunck(1, 0);
	AddChunck(0, 1);
	AddChunck(1, 1);
	AddChunck(1, 2);
}

void ARCWorldManager::AddChunck(int X, int Y)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector ChunckSpawnLocation = FVector( X * WorldSettings->WorldChunckSize, Y * WorldSettings->WorldChunckSize, 0);
	
	ARCWorldChunck* NewChunk = GetWorld()->SpawnActor<ARCWorldChunck>(
		ChunksClass, 
		ChunckSpawnLocation, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	AllChunks.Emplace(FVector2D(X, Y) ,NewChunk);
}
