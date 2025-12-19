// Fill out your copyright notice in the Description page of Project Settings.


#include "RCBlock.h"

// Sets default values
ARCBlock::ARCBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_WorldStatic);
	StaticMesh->SetupAttachment(GetRootComponent());
}

void ARCBlock::OnInteract(AActor* InteractActor)
{
	UE_LOG(LogTemp, Warning, TEXT("ARCBlock::OnInteract"));
}

