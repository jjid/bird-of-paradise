// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSpawner.h"

// Sets default values
ABossSpawner::ABossSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABossSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	time += DeltaTime;

	if (time > 60)
	{
		round++;
		if ((round + 1) % 5 == 0)
		{
			if (round + 1 == 5)
			{
				SpawnEnemyAtPoint(0);
			}
			else if (round + 1 == 10)
			{
				SpawnEnemyAtPoint(1);
			}
			else if (round + 1 == 15)
			{
				SpawnEnemyAtPoint(2);
			}
			else if (round + 1 == 20)
			{
				SpawnEnemyAtPoint(3);
			}
			else if (round + 1 == 25)
			{
				SpawnEnemyAtPoint(4);
			}
			else if (round + 1 >= 30)
			{
				SpawnEnemyAtPoint(5);
			}
		}
		time = 0;
	}
}


void ABossSpawner::SpawnEnemyAtPoint(int tempInt)
{
	
	if (!EnemyBlueprintClass)
	{
		return;
	}
	
	FVector SpawnLocation = GetActorLocation();;
	FRotator SpawnRotation = GetActorRotation();;

	if (tempInt == 0)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass, SpawnLocation, SpawnRotation);
	}
	else if (tempInt == 1)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass2, SpawnLocation, SpawnRotation);
	}
	else if (tempInt == 2)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass3, SpawnLocation, SpawnRotation);
	}
	else if (tempInt == 3)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass4, SpawnLocation, SpawnRotation);
	}
	else if (tempInt == 4)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass5, SpawnLocation, SpawnRotation);
	}
	else if (tempInt == 5)
	{
		GetWorld()->SpawnActor<AActor>(EnemyBlueprintClass6, SpawnLocation, SpawnRotation);
	} 
}
