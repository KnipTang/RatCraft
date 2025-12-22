// Fill out your copyright notice in the Description page of Project Settings.


#include "RCBlock.h"

#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARCBlock::ARCBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//StaticMesh->SetCollisionObjectType(ECC_WorldStatic);
	//StaticMesh->SetupAttachment(GetRootComponent());

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = ProceduralMesh;
}

void ARCBlock::Init(const FVector& GridCoords)
{
	GridCoordinates = GridCoords;
	CreateBlock();
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

void ARCBlock::CreateBlock()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;

	float Half = BlockLength * 0.5f;
	
    TArray<FVector> CubeVertices = {
        FVector(-Half, -Half, -Half), // Front-bottom-left
        FVector(Half, -Half, -Half),  // Front-bottom-right
        FVector(Half, -Half, Half),   // Front-top-right
        FVector(-Half, -Half, Half),  // Front-top-left
        FVector(-Half, Half, -Half),  // Back-bottom-left
        FVector(Half, Half, -Half),   // Back-bottom-right
        FVector(Half, Half, Half),    // Back-top-right
        FVector(-Half, Half, Half)    // Back-top-left
    };
	
    TArray<TArray<int32>> Faces = {
        {3, 2, 1, 0}, // South
        {6, 7, 4, 5}, // North
        {7, 3, 0, 4}, // West
        {2, 6, 5, 1}, // East
        {7, 6, 2, 3}, // Top
        {0, 1, 5, 4}  // Bottom
    };
	
    TArray<FVector> FaceNormals = {
        FVector(0, -1, 0),  // South
        FVector(0, 1, 0),   // North
        FVector(-1, 0, 0),  // West
        FVector(1, 0, 0),   // East
        FVector(0, 0, 1),   // Top
        FVector(0, 0, -1)   // Bottom
    };
	
    for (int32 FaceIndex = 0; FaceIndex < Faces.Num(); FaceIndex++)
    {
        const TArray<int32>& Face = Faces[FaceIndex];
        int32 BaseIndex = Vertices.Num();

        for (int32 i = 0; i < 4; i++)
        {
            Vertices.Add(CubeVertices[Face[i]]);
        	
            if (i == 0) UVs.Add(FVector2D(0, 1));
            else if (i == 1) UVs.Add(FVector2D(1, 1));
            else if (i == 2) UVs.Add(FVector2D(1, 0));
            else UVs.Add(FVector2D(0, 0));
        	
            VertexColors.Add(FColor::White);
        }
    	
        Triangles.Add(BaseIndex);
        Triangles.Add(BaseIndex + 1);
        Triangles.Add(BaseIndex + 2);
        
        Triangles.Add(BaseIndex);
        Triangles.Add(BaseIndex + 2);
        Triangles.Add(BaseIndex + 3);
    	
        for (int32 i = 0; i < 4; i++)
        {
            Normals.Add(FaceNormals[FaceIndex]);
        }
    }
	
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);
	
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
}

