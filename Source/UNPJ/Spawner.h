// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class UNPJ_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnEnemyAtPoint(int tempInt);
	void SpawnTumbleweedAbovePlayer();

	float time = 0.0f;
	float spawnTime = 0.0f;
	int round = 0;
	int MaxSpawnCycle[7] = {20, 15, 14, 13, 11, 9, 6};
	int MinSpawnCycle[7] = {15, 13, 12, 10, 8, 7, 4};
	int randomSpawnCycle = 0;

	bool SpawnCheck = false;
	
	float RandomFloat = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EnemyBlueprintClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EnemyBlueprintClass2;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EnemyBlueprintClass3;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EnemyBlueprintClass4;
};
