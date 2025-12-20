// Fill out your copyright notice in the Description page of Project Settings.


#include "RCBlock.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ARCBlock::ARCBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_WorldStatic);
	StaticMesh->SetupAttachment(GetRootComponent());
}

void ARCBlock::Init(const FVector& GridCoords)
{
	GridCoordinates = GridCoords;
}

void ARCBlock::OnInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("ARCBlock::OnInteract"));

	if (!bIsMining)
	{
		StartMining();
	}
}

void ARCBlock::EndInteract()
{
	StopMining();
}

void ARCBlock::StartMining()
{
	bIsMining = true;

	if (UWorld* World = GetWorld())
		World->GetTimerManager().SetTimer(
		MiningTimerHandle,
		this,
		&ARCBlock::UpdateMiningProgress,
		MiningUpdateInterval,
		true
	);
	
}

void ARCBlock::UpdateMiningProgress()
{
	CurrentMinedTime += MiningUpdateInterval;

	UE_LOG(LogTemp, Warning, TEXT("%f"), 
		CurrentMinedTime);
	
	if (CurrentMinedTime >= MineTime)
	{
		OnBlockMined();
	}
}

void ARCBlock::StopMining()
{
	bIsMining = false;
	CurrentMinedTime = 0;

	UE_LOG(LogTemp, Warning, TEXT("STOP"));


	if (UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(MiningTimerHandle);
}

void ARCBlock::OnBlockMined()
{
	StopMining();
	UE_LOG(LogTemp, Warning, TEXT("BlockGotMined"));
	StaticMesh->SetVisibility(false);
}

