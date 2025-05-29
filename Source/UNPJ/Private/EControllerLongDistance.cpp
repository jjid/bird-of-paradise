#include "EControllerLongDistance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyCharacter.h"

void AEControllerLongDistance::ResetCharge()
{
    bCanCharge = true;
}

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

    // 돌진 조건
    if (!bIsCharging && bCanCharge && Distance <= ChargeTriggerDistance)
    {
        bIsCharging = true;
        bCanCharge = false;

        StopMovement();
        ClearFocus(EAIFocusPriority::Default);

        FVector Direction = (PlayerPawn->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal();
        FVector LaunchVelocity = Direction * ChargeSpeed;

        Enemy->GetCharacterMovement()->GravityScale = 0.f;
        Enemy->PlayWalkAnimation(); // 돌진 애니메이션
        Enemy->LaunchCharacter(LaunchVelocity, true, true);

        GetWorld()->GetTimerManager().SetTimer(
            ChargeCooldownTimerHandle, this, &AEControllerLongDistance::ResetCharge, ChargeCooldownTime, false
        );
    }

    // 빙글빙글 회전 (Garen E 스타일)
    if (bIsCharging && ControlledPawn)
    {
        FRotator CurrentRotation = ControlledPawn->GetActorRotation();
        float DeltaYaw = SpinSpeed * DeltaSeconds;
        FRotator NewRotation = FRotator(0.f, CurrentRotation.Yaw + DeltaYaw, 0.f);
        ControlledPawn->SetActorRotation(NewRotation);
    }

    // 돌진 종료 조건: 플레이어에게서 멀어짐
    if (bIsCharging && Distance > 1500.f)
    {
        bIsCharging = false;
        bIsMoving = false;

        Enemy->GetCharacterMovement()->GravityScale = 1.f;
        Enemy->PlayIdleAnimation();

        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
    }

    // 평소 추적
    if (!bIsCharging && !bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Enemy->PlayWalkAnimation();
        Enemy->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        bIsMoving = true;
    }

    // 도착해서 멈춤
    if (bIsCloseEnough && Speed < 5.f)
    {
        if (bIsMoving || bIsCharging)
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Default);
            Enemy->PlayIdleAnimation();

            bIsMoving = false;
            bIsCharging = false;
            Enemy->GetCharacterMovement()->GravityScale = 1.f;
        }
    }
}
