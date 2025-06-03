#include "JumpController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "EnemyCharacter.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"

void AJumpController::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn && GetPawn())
    {
        SetFocus(PlayerPawn);
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);

        if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
        {
            Enemy->PlayWalkAnimation();
        }

        bIsMoving = true;
    }
}

void AJumpController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
    float Speed = ControlledPawn->GetVelocity().Size();

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ControlledPawn);
    if (!Enemy) return;

    if (!bIsJumpAttacking && Distance <= AttackTriggerDistance)
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Default);
        bIsJumpAttacking = true;

        if (ACharacter* Character = Cast<ACharacter>(Enemy))
        {
            FVector JumpImpulse = FVector(0.f, 0.f, 10000.f); // 위로 점프
            Character->LaunchCharacter(JumpImpulse, true, true);
        }

        Enemy->PlayJumpAttackAnimation();

        UE_LOG(LogTemp, Warning, TEXT(" 점프 어택 실행: 거리 %.1f"), Distance);

        GetWorld()->GetTimerManager().SetTimer(
            JumpAttackCooldownHandle,
            this,
            &AJumpController::OnJumpAttackFinished,
            2.0f, false
        );
        return;
    }

    // 점프 중이면 아무 것도 안 함
    if (bIsJumpAttacking) return;

    const bool bIsCloseEnough = Distance <= AcceptanceRadius;
    const bool bIsStopped = Speed < 5.f;

    if (bIsCloseEnough && bIsStopped)
    {
        if (bIsMoving)
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Default);
            Enemy->PlayIdleAnimation();
            bIsMoving = false;

            UE_LOG(LogTemp, Warning, TEXT(" 멈춤 + Idle 전환: 거리 %.1f, 속도 %.1f"), Distance, Speed);
        }
    }
    else if (!bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Enemy->PlayWalkAnimation();
        bIsMoving = true;

        UE_LOG(LogTemp, Warning, TEXT(" 다시 추적 시작: 거리 %.1f, 속도 %.1f"), Distance, Speed);
    }
}

void AJumpController::OnJumpAttackFinished()
{
    bIsJumpAttacking = false;

    if (PlayerPawn)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);

        if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
        {
            Enemy->PlayWalkAnimation();
        }

        bIsMoving = true;
    }
}
