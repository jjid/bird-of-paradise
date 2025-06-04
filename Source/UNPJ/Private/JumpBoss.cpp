#include "JumpBoss.h"
#include "Kismet/GameplayStatics.h"
#include "BossCharacter.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

void AJumpBoss::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (PlayerPawn && GetPawn())
    {
        SetFocus(PlayerPawn);
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);

        ABossCharacter* Boss = Cast<ABossCharacter>(GetPawn());
        if (Boss)
        {
            Boss->PlayWalkAnimation();
        }

        bIsMoving = true;
    }
}

void AJumpBoss::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
    float Speed = ControlledPawn->GetVelocity().Size();

    ABossCharacter* Boss = Cast<ABossCharacter>(ControlledPawn);
    if (!Boss) return;

    const bool bIsCloseEnough = Distance <= AcceptanceRadius;
    const bool bInJumpRange = Distance <= JumpAttackDistance;
    const bool bIsStopped = Speed < 5.f;

    if (bInJumpRange && !bIsJumpAttacking)
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Default);
        bIsJumpAttacking = true;

        PerformJumpAttack();

        GetWorld()->GetTimerManager().SetTimer(JumpAttackCooldownHandle, [this]()
            {
                bIsJumpAttacking = false;
                UE_LOG(LogTemp, Warning, TEXT("Jump cooldown ended"));
            }, JumpAttackCooldown, false);

        return;
    }

    if (bIsCloseEnough && bIsStopped)
    {
        if (bIsMoving)
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Default);
            Boss->PlayIdleAnimation();
            bIsMoving = false;
        }
    }
    else if (!bIsCloseEnough && !bIsJumpAttacking)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Boss->PlayWalkAnimation();
        bIsMoving = true;
    }
}

void AJumpBoss::PerformJumpAttack()
{
    ACharacter* ControlledChar = Cast<ACharacter>(GetPawn());
    if (!ControlledChar || !PlayerPawn) return;

    FVector StartLoc = ControlledChar->GetActorLocation();
    FVector TargetLoc = PlayerPawn->GetActorLocation();

    FVector Dir = (TargetLoc - StartLoc).GetSafeNormal2D();
    FVector LaunchVelocity = Dir * 1000.f + FVector(0.f, 0.f, 800.f);

    ControlledChar->SetActorRotation(Dir.Rotation());
    ControlledChar->LaunchCharacter(LaunchVelocity, true, true);

    UE_LOG(LogTemp, Warning, TEXT("Boss jump velocity: %s"), *LaunchVelocity.ToString());

    GetWorld()->GetTimerManager().SetTimer(JumpAttackCooldownHandle, [this]()
        {
            ABossCharacter* Boss = Cast<ABossCharacter>(GetPawn());
            if (Boss)
            {
                Boss->PlaySmashAttackAnimation();
            }

            bIsJumpAttacking = false;

            if (PlayerPawn && GetPawn())
            {
                MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
                SetFocus(PlayerPawn);
            }

        }, 1.0f, false);
}
