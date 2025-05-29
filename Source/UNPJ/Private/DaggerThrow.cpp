#include "DaggerThrow.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

void ADaggerThrow::ResetThrow()
{
    bCanThrow = true;
    bIsThrowing = false;
}

void ADaggerThrow::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ADaggerThrow::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ControlledPawn);
    if (!Enemy) return;

    float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerPawn->GetActorLocation());

    if (Distance <= ThrowTriggerDistance)
    {
        StopMovement();

        if (!bIsThrowing)
        {
            bIsThrowing = true;
            Enemy->PlayThrowAnimation();
        }

        if (bCanThrow && DaggerProjectileClass)
        {
            bCanThrow = false;

            FVector SpawnLocation = ControlledPawn->GetActorLocation() + ControlledPawn->GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
            FRotator SpawnRotation = (PlayerPawn->GetActorLocation() - SpawnLocation).Rotation();

            FActorSpawnParameters Params;
            Params.Owner = ControlledPawn;
            Params.Instigator = ControlledPawn;

            GetWorld()->SpawnActor<AActor>(DaggerProjectileClass, SpawnLocation, SpawnRotation, Params);

            GetWorld()->GetTimerManager().SetTimer(ThrowCooldownTimerHandle, this, &ADaggerThrow::ResetThrow, ThrowCooldownTime, false);
        }
    }
    else
    {
        MoveToActor(PlayerPawn, AcceptanceRadius, true);

        if (Enemy)
        {
            Enemy->PlayWalkAnimation();
        }

        bIsThrowing = false;
    }
}
