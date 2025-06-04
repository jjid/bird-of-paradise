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

    // 부드러운 버로우 중이라면 Z보간만 처리
    if (bIsBurrowing)
    {
        BurrowElapsed += DeltaSeconds;
        float Alpha = FMath::Clamp(BurrowElapsed / BurrowInterpTime, 0.f, 1.f);
        FVector Loc = EnemyChar->GetActorLocation();
        Loc.Z = FMath::Lerp(BurrowStartZ, BurrowEndZ, Alpha);
        EnemyChar->SetActorLocation(Loc);

        if (Alpha >= 1.0f)
        {
            bIsBurrowing = false;
            EnemyChar->SetActorHiddenInGame(true);
            EnemyChar->SetActorEnableCollision(false);

            GetWorld()->GetTimerManager().SetTimer(BurrowTimerHandle, this, &ABurrowController::ExecuteAmbush, BurrowDelay, false);
        }

        return;
    }

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

    FVector StartLoc = EnemyChar->GetActorLocation();
    BurrowStartZ = StartLoc.Z;
    BurrowEndZ = BurrowStartZ - 200.f;

    BurrowElapsed = 0.0f;
    bIsBurrowing = true;
    bIsBurrowed = true;
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
