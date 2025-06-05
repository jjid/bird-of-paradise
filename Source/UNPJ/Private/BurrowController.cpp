#include "BurrowController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.h"
#include "UNPJ/UNPJCharacter.h"
#include "TimerManager.h"

void ABurrowController::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    PlayerCharacter = Cast<AUNPJCharacter>(PlayerPawn);

    // 오버랩 델리게이트 제거됨
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

    if (Distance <= AttackRange)
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Default);
        StartBurrow();
        return;
    }

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

    bBurrowAttackDamage = false;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyChar);
    if (Enemy)
    {
        Enemy->PlaySmashAttackAnimation();
    }

    float HitDistance = FVector::Dist(EnemyChar->GetActorLocation(), PlayerPawn->GetActorLocation());

    if (!bBurrowAttackDamage && HitDistance <= AmbushHitRadius)
    {
        bBurrowAttackDamage = true;
        if (PlayerCharacter)
        {
            PlayerCharacter->SetHP(-10.f);
        }
    }

    bIsBurrowed = false;
    bIsInCooldown = true;

    GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
        {
            bIsInCooldown = false;
        }, AttackCooldown, false);
}
