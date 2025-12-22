// Fill out your copyright notice in the Description page of Project Settings.


#include "RCAbilitySystemStatics.h"

FHitResult URCAbilitySystemStatics::GetHitscanTarget(const UWorld* World, const FVector& EyesLocation, const FRotator& EyesRotation, ECollisionChannel CollisionType, float LookDistance, bool DebugLine)
{
	FHitResult HitResult;
	
	const FVector AimEnd = EyesLocation + EyesRotation.Vector() * LookDistance;

	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(CollisionType);

	FCollisionQueryParams CollisionQueryParams;
	
	if (DebugLine)
	{
		DrawDebugLine(World, EyesLocation, AimEnd, FColor::Red, false, 2.f, 0U, 3.f);
	}
		
	World->LineTraceSingleByObjectType(HitResult, EyesLocation, AimEnd, CollisionObjectQueryParams, CollisionQueryParams);

	return HitResult;
}
