#include "BurrowController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.h"
#include "TimerManager.h"

void ABurrowController::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ABurrowController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!EnemyChar) return;

    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), EnemyChar->GetActorLocation());

    if (bIsBurrowed || bIsInCooldown)
        return;

    // 플레이어가 사거리 내에 있음
    if (Distance <= AttackRange)
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Default);
        StartBurrow();
        return;
    }

    // 평상시 추적
    if (!bIsMoving || Distance > AcceptanceRadius)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 50, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);

        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyChar);
        if (Enemy)
        {
            Enemy->PlayWalkAnimation();
        }

        bIsMoving = true;
    }
    else if (Distance <= AcceptanceRadius)
    {
        StopMovement();
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyChar);
        if (Enemy)
        {
            Enemy->PlayIdleAnimation();
        }

        bIsMoving = false;
    }
}

void ABurrowController::StartBurrow()
{
    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!EnemyChar) return;

    EnemyChar->SetActorHiddenInGame(true);
    EnemyChar->SetActorEnableCollision(false);
    bIsBurrowed = true;


    GetWorld()->GetTimerManager().SetTimer(BurrowTimerHandle, this, &ABurrowController::ExecuteAmbush, BurrowDelay, false);
}

void ABurrowController::ExecuteAmbush()
{
    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!EnemyChar || !PlayerPawn) return;

    FVector PlayerLoc = PlayerPawn->GetActorLocation();
    PlayerLoc.Z += 10.f;

    EnemyChar->SetActorLocation(PlayerLoc);
    EnemyChar->SetActorHiddenInGame(false);
    EnemyChar->SetActorEnableCollision(true);

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyChar);
    if (Enemy)
    {
        Enemy->PlaySmashAttackAnimation();
    }

    bIsBurrowed = false;
    bIsInCooldown = true;


    GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
        {
            bIsInCooldown = false;
            UE_LOG(LogTemp, Warning, TEXT("쿨타임 끝"));
        }, AttackCooldown, false);
}
