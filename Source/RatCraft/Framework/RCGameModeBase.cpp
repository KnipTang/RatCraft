// Fill out your copyright notice in the Description page of Project Settings.


#include "RCGameModeBase.h"

#include "RatCraft/World/RCWorldGenerator.h"
#include "RatCraft/World/Grid/RCGrid.h"

ARCGameModeBase::ARCGameModeBase()
{
}

void ARCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	WorldGenerator = GetWorld()->SpawnActor<ARCWorldGenerator>(
		WorldGeneratorClass, 
		FVector(0, 0, 0), 
		FRotator::ZeroRotator, 
		SpawnParams
	);
}

class ARCGrid* ARCGameModeBase::GetGrid() const
{
	return WorldGenerator->GetGrid();
}
