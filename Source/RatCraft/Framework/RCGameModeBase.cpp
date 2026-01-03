// Fill out your copyright notice in the Description page of Project Settings.


#include "RCGameModeBase.h"
#include "RatCraft/World/RCWorldManager.h"

ARCGameModeBase::ARCGameModeBase()
{
}

void ARCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	WorldManager = GetWorld()->SpawnActor<ARCWorldManager>(
		WorldManagerClass, 
		FVector(0, 0, 0), 
		FRotator::ZeroRotator, 
		SpawnParams
	);
}