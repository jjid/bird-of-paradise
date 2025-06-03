// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossSpawner.generated.h"

UCLASS()
class UNPJ_API ABossSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SpawnEnemyAtPoint(int tempInt);
	
	int round = 0;
	float time = 0.0f;
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

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EnemyBlueprintClass5;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> EnemyBlueprintClass6;
};
