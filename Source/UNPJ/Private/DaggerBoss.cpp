#include "DaggerBoss.h"
#include "Kismet/GameplayStatics.h"
#include "BossCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

void ADaggerBoss::ResetThrow()
{
    bCanThrow = true;
    bIsThrowing = false;
}

void ADaggerBoss::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ADaggerBoss::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    ABossCharacter* Boss = Cast<ABossCharacter>(ControlledPawn);
    if (!Boss) return;

    float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerPawn->GetActorLocation());

    if (Distance <= ThrowTriggerDistance)
    {
        StopMovement();

        if (!bIsThrowing)
        {
            bIsThrowing = true;
            Boss->PlayThrowAnimation();
        }

        if (bCanThrow && DaggerProjectileClass)
        {
            bCanThrow = false;

            FVector SpawnOrigin = ControlledPawn->GetActorLocation() + FVector(0, 0, 50.f);
            FVector TargetLocation = PlayerPawn->GetActorLocation();

            // 여러 발 생성
            const int32 NumDaggers = 4;
            const float SpreadAngle = 15.0f; // 각도 차이

            FVector Direction = (TargetLocation - SpawnOrigin).GetSafeNormal();
            FRotator BaseRot = Direction.Rotation();

            for (int32 i = 0; i < NumDaggers; ++i)
            {
                float OffsetYaw = SpreadAngle * (i - NumDaggers / 2);
                FRotator SpreadRot = BaseRot + FRotator(0, OffsetYaw, 0);

                FVector SpawnLoc = SpawnOrigin + SpreadRot.Vector() * 100.f;

                FActorSpawnParameters Params;
                Params.Owner = ControlledPawn;
                Params.Instigator = ControlledPawn;

                GetWorld()->SpawnActor<AActor>(DaggerProjectileClass, SpawnLoc, SpreadRot, Params);
            }

            GetWorld()->GetTimerManager().SetTimer(ThrowCooldownTimerHandle, this, &ADaggerBoss::ResetThrow, ThrowCooldownTime, false);
        }
    }
    else
    {
        MoveToActor(PlayerPawn, AcceptanceRadius, true);

        if (Boss)
        {
            Boss->PlayWalkAnimation();
        }

        bIsThrowing = false;
    }
}
