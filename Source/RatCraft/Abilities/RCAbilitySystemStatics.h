// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RCAbilitySystemStatics.generated.h"
/**
 * 
 */
UCLASS()
class RATCRAFT_API URCAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FHitResult GetHitscanTarget(const UWorld* World, const FVector& EyesLocation, const FRotator& EyesRotation, ECollisionChannel CollisionType, float LookDistance = 100, bool DebugLine = false);
};
