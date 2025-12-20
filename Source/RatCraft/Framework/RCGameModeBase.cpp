// Fill out your copyright notice in the Description page of Project Settings.


#include "RCGameModeBase.h"

#include "RatCraft/World/Grid/RCGrid.h"

ARCGameModeBase::ARCGameModeBase()
{
}

void ARCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	Grid = GetWorld()->SpawnActor<ARCGrid>(
		GridClass, 
		FVector(0, 0, 0), 
		FRotator::ZeroRotator, 
		SpawnParams
	);
}
