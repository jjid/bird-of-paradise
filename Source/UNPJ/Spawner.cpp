// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"
#include "Spawner.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/TargetPoint.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	randomSpawnCycle = FMath::RandRange(MinSpawnCycle[round / 5], MaxSpawnCycle[round / 5]);
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	time += DeltaTime;

	if (time > 60)
	{
		round++;
		time = 0;
	}
	
	if (round > 29)
	{
		round = 30;
	}
	
	spawnTime += DeltaTime;

	if (spawnTime > randomSpawnCycle)
	{
		randomSpawnCycle = FMath::RandRange(MinSpawnCycle[round / 5], MaxSpawnCycle[round / 5]);
		SpawnCheck = true;
		spawnTime = 0;
	}
	
	if ((round+1) % 5 != 0)
	{
		if (SpawnCheck)
		{
			RandomFloat = FMath::FRandRange(1.f, 100.f);
			if (RandomFloat < 0)
			{
				SpawnEnemyAtPoint(0);
			}
			else if (RandomFloat < 33)
			{
				SpawnEnemyAtPoint(1);
			}
			else if (RandomFloat < 66)
			{
				SpawnEnemyAtPoint(2);
			}
			else if (RandomFloat < 100)
			{
				SpawnEnemyAtPoint(3);
			}
			SpawnCheck = false;
		}
	} 
}

void ASpawner::SpawnEnemyAtPoint(int tempInt)
{
	
	if (!EnemyBlueprintClass)
	{
		SpawnTumbleweedAbovePlayer();
		return;
	}
	
	FVector SpawnLocation = GetActorLocation();;
	FRotator SpawnRotation = GetActorRotation();;

	if (tempInt == 0)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass, SpawnLocation, SpawnRotation);
	} else if (tempInt == 1)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass2, SpawnLocation, SpawnRotation);
	} else if (tempInt == 2)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass3, SpawnLocation, SpawnRotation);
	} else if (tempInt == 3)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass4, SpawnLocation, SpawnRotation);
	} 
}

void ASpawner::SpawnTumbleweedAbovePlayer()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn) return;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector SpawnLocation = PlayerLocation + FVector(0, 0, 200); // 머리 위 200유닛

	FRotator SpawnRotation = FRotator::ZeroRotator;

	// StaticMeshActor 생성
	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnLocation, SpawnRotation);
	if (MeshActor)
	{
		UStaticMeshComponent* MeshComp = MeshActor->GetStaticMeshComponent();

		// 💡 먼저 Mobility 설정
		MeshComp->SetMobility(EComponentMobility::Movable);

		// 메시 할당
		UStaticMesh* TumbleweedMesh = Cast<UStaticMesh>(
			StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Location_Western/Meshes/SM_tumbleweed_001.SM_tumbleweed_001'"))
		);

		if (TumbleweedMesh)
		{
			MeshComp->SetStaticMesh(TumbleweedMesh);
			MeshActor->SetActorEnableCollision(false);
		}
	}
}
