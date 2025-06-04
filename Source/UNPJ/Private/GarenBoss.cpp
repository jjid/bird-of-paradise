#include "GarenBoss.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BossCharacter.h"

void AGarenBoss::ResetCharge()
{
    bCanCharge = true;
}

void AGarenBoss::BeginPlay()
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
            Boss->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        }

        bIsMoving = true;
    }
}

void AGarenBoss::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    ABossCharacter* Boss = Cast<ABossCharacter>(ControlledPawn);
    if (!Boss) return;

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

        Boss->GetCharacterMovement()->GravityScale = 0.f;
        Boss->PlayWalkAnimation();
        Boss->LaunchCharacter(LaunchVelocity, true, true);

        GetWorld()->GetTimerManager().SetTimer(
            ChargeCooldownTimerHandle, this, &AGarenBoss::ResetCharge, ChargeCooldownTime, false
        );
    }

    // 회전
    if (bIsCharging && ControlledPawn)
    {
        FRotator CurrentRotation = ControlledPawn->GetActorRotation();
        float DeltaYaw = SpinSpeed * DeltaSeconds;
        FRotator NewRotation = FRotator(0.f, CurrentRotation.Yaw + DeltaYaw, 0.f);
        ControlledPawn->SetActorRotation(NewRotation);
    }

    // 돌진 종료 조건
    if (bIsCharging && Distance > 1500.f)
    {
        bIsCharging = false;
        bIsMoving = false;

        Boss->GetCharacterMovement()->GravityScale = 1.f;
        Boss->PlayIdleAnimation();

        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
    }

    // 추적
    if (!bIsCharging && !bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Boss->PlayWalkAnimation();
        Boss->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        bIsMoving = true;
    }

    // 멈춤
    if (bIsCloseEnough && Speed < 5.f)
    {
        if (bIsMoving || bIsCharging)
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Default);
            Boss->PlayIdleAnimation();

            bIsMoving = false;
            bIsCharging = false;
            Boss->GetCharacterMovement()->GravityScale = 1.f;
        }
    }
}
