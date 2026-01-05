// Fill out your copyright notice in the Description page of Project Settings.


#include "RCBlock.h"

// Sets default values
ARCBlock::ARCBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CubeMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = CubeMesh;
}

// Called when the game starts or when spawned
void ARCBlock::BeginPlay()
{
	Super::BeginPlay();
}