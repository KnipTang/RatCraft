// Fill out your copyright notice in the Description page of Project Settings.


#include "RCBlock.h"

#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "RCDataAssetBlock.h"

// Sets default values
ARCBlock::ARCBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = ProceduralMesh;
}

void ARCBlock::Init(class URCDataAssetBlock* DataAsset, const EBlockType BlockTypeToSpawn, const FVector& GridCoords)
{
	DataAssetBlock = DataAsset;
	GridCoordinates = GridCoords;

	if (GridCoordinates.Z != 0)
		bIsMinable = true;

	if(BlockTypeToSpawn != EBlockType::Air)
		CreateMesh(BlockTypeToSpawn);
	ConfigureBlock(BlockTypeToSpawn);
}

void ARCBlock::OnInteract()
{
	if (bIsMinable && !bIsMining)
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
	
	if (CurrentMinedTime >= DataAssetBlock->GetMineTime())
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
	ConfigureBlock(EBlockType::Air);
}

void ARCBlock::CreateMesh(const EBlockType BlockTypeToSpawn)
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
        	
            VertexColors.Add(GetBlockColorFromBlockType(BlockTypeToSpawn));
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

	if (UMaterialInterface* Material = DataAssetBlock->GetMaterial())
	{
		ProceduralMesh->SetMaterial(0, Material);
	}
}

void ARCBlock::ConfigureBlock(const EBlockType BlockTypeToSpawn)
{
	switch (BlockTypeToSpawn)
	{
		case EBlockType::Air:
			ProceduralMesh->SetVisibility(false);
			ProceduralMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		break;
		default:
			ProceduralMesh->SetVisibility(true);
			ProceduralMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

FColor ARCBlock::GetBlockColorFromBlockType(const EBlockType BlockTypeToSpawn)
{
	switch (BlockTypeToSpawn)
	{
		case EBlockType::Air:
			break;
		case EBlockType::Grass:
			return FColor::Green;
			break;
		case EBlockType::Stone:
			return FColor::Black;
			break;
		case EBlockType::Snow:
			return FColor::White;
			break;
	}
	
	return FColor::Purple;
}

