// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RCBlock.generated.h"

UCLASS()
class RATCRAFT_API ARCBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARCBlock();

	void ToggleVisibility(const bool Visibility) const { CubeMesh->SetVisibility(Visibility); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;
};
