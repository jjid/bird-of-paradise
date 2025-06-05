#include "GarenBoss.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BossCharacter.h"
#include "UNPJ/UNPJCharacter.h"
#include "TimerManager.h"

void AGarenBoss::ResetCharge()
{
    bCanCharge = true;
    bAttackDamage = true;
}

void AGarenBoss::EndCharge()
{
    if (!bIsCharging || !GetPawn()) return;

    bIsCharging = false;
    bIsMoving = false;

    ABossCharacter* Boss = Cast<ABossCharacter>(GetPawn());
    if (Boss)
    {
        Boss->GetCharacterMovement()->Velocity = FVector::ZeroVector;
        Boss->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        Boss->GetCharacterMovement()->GravityScale = 1.f;
        Boss->PlayIdleAnimation();

        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
    }
}

void AGarenBoss::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    PlayerCharacter = Cast<AUNPJCharacter>(PlayerPawn);

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
        bAttackDamage = false;

        StopMovement();
        ClearFocus(EAIFocusPriority::Default);

        FVector BossLocation = ControlledPawn->GetActorLocation();
        FVector PlayerLocation = PlayerPawn->GetActorLocation();

        FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
        FVector LaunchVelocity = Direction * ChargeSpeed;

        FRotator NewRotation = Direction.Rotation();
        ControlledPawn->SetActorRotation(NewRotation);

        Boss->GetCharacterMovement()->StopMovementImmediately();
        Boss->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        Boss->GetCharacterMovement()->GravityScale = 0.f;
        Boss->GetCharacterMovement()->Velocity = LaunchVelocity;

        Boss->PlayWalkAnimation();

        GetWorld()->GetTimerManager().SetTimer(
            ChargeCooldownTimerHandle, this, &AGarenBoss::ResetCharge, ChargeCooldownTime, false
        );

        GetWorld()->GetTimerManager().SetTimer(
            ChargeEndTimerHandle, this, &AGarenBoss::EndCharge, ChargeDurationTime, false
        );
    }

    // 돌진 중 회전 연출
    if (bIsCharging)
    {
        FRotator CurrentRotation = ControlledPawn->GetActorRotation();
        float DeltaYaw = SpinSpeed * DeltaSeconds;
        FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + DeltaYaw, CurrentRotation.Roll);
        ControlledPawn->SetActorRotation(NewRotation);
    }

    // 직접 충돌 거리 판정
    if (bIsCharging && !bAttackDamage && PlayerCharacter)
    {
        float HitDistance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
        if (HitDistance <= 700.f) // 충돌 거리 (필요 시 조절)
        {
            bAttackDamage = true;
            PlayerCharacter->SetHP(-20.f); // 원하는 데미지
        }
    }

    // 너무 멀면 강제 종료
    if (bIsCharging && Distance > 1500.f)
    {
        EndCharge();
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
            Boss->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
            Boss->GetCharacterMovement()->Velocity = FVector::ZeroVector;

            GetWorld()->GetTimerManager().ClearTimer(ChargeEndTimerHandle);
        }
    }
}
