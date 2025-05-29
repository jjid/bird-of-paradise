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

    // ���� ����
    if (!bIsCharging && bCanCharge && Distance <= ChargeTriggerDistance)
    {
        bIsCharging = true;
        bCanCharge = false;

        StopMovement();
        ClearFocus(EAIFocusPriority::Default);

        FVector Direction = (PlayerPawn->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal();
        FVector LaunchVelocity = Direction * ChargeSpeed;

        Enemy->GetCharacterMovement()->GravityScale = 0.f;
        Enemy->PlayWalkAnimation(); // ���� �ִϸ��̼�
        Enemy->LaunchCharacter(LaunchVelocity, true, true);

        GetWorld()->GetTimerManager().SetTimer(
            ChargeCooldownTimerHandle, this, &AEControllerLongDistance::ResetCharge, ChargeCooldownTime, false
        );
    }

    // ���ۺ��� ȸ�� (Garen E ��Ÿ��)
    if (bIsCharging && ControlledPawn)
    {
        FRotator CurrentRotation = ControlledPawn->GetActorRotation();
        float DeltaYaw = SpinSpeed * DeltaSeconds;
        FRotator NewRotation = FRotator(0.f, CurrentRotation.Yaw + DeltaYaw, 0.f);
        ControlledPawn->SetActorRotation(NewRotation);
    }

    // ���� ���� ����: �÷��̾�Լ� �־���
    if (bIsCharging && Distance > 1500.f)
    {
        bIsCharging = false;
        bIsMoving = false;

        Enemy->GetCharacterMovement()->GravityScale = 1.f;
        Enemy->PlayIdleAnimation();

        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
    }

    // ��� ����
    if (!bIsCharging && !bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Enemy->PlayWalkAnimation();
        Enemy->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        bIsMoving = true;
    }

    // �����ؼ� ����
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
