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
	float round = 1;
	float SpawnCycle = 0;
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
