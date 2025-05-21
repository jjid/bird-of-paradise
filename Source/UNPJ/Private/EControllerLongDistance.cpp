#include "EControllerLongDistance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "EnemyCharacter.h"

void AEControllerLongDistance::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn && GetPawn())
    {
        SetFocus(PlayerPawn);
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);

        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
        if (Enemy)
        {
            Enemy->PlayWalkAnimation();

            // 기본 걷기 속도 설정
            Enemy->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        }

        bIsMoving = true;
    }
}
void AEControllerLongDistance::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ControlledPawn);
    if (!Enemy) return;

    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
    float Speed = ControlledPawn->GetVelocity().Size();

    const bool bIsCloseEnough = Distance <= AcceptanceRadius;
    const bool bIsStopped = Speed < 5.f;

    if (!bIsCharging && Distance <= ChargeTriggerDistance)
    {
        bIsCharging = true;

        StopMovement();
        ClearFocus(EAIFocusPriority::Default);

        FVector Direction = (PlayerPawn->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal();
        FVector LaunchVelocity = Direction * ChargeSpeed;

        Enemy->GetCharacterMovement()->GravityScale = 0.f;

        Enemy->PlayWalkAnimation(); // 돌진 애니가 있으면 변경
        Enemy->LaunchCharacter(LaunchVelocity, true, true);

    }
    else if (!bIsCloseEnough && !bIsCharging)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Enemy->PlayWalkAnimation();
        Enemy->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        bIsMoving = true;
    }
    else if (bIsCloseEnough && bIsStopped)
    {
        if (bIsMoving || bIsCharging)
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Default);
            Enemy->PlayIdleAnimation();

            bIsMoving = false;
            bIsCharging = false;

            // 다시 중력 복구
            Enemy->GetCharacterMovement()->GravityScale = 1.f;
        }
    }
}
