#include "BasicBoss.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "BossCharacter.h"
#include "UNPJ/UNPJCharacter.h"
#include "NavigationSystem.h"

void ABasicBoss::BeginPlay()
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

    PlayerCharacter = Cast<AUNPJCharacter>(PlayerPawn);
}

void ABasicBoss::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
    float Speed = ControlledPawn->GetVelocity().Size();

    ABossCharacter* Boss = Cast<ABossCharacter>(ControlledPawn);
    if (!Boss) return;

    const bool bIsCloseEnough = Distance <= AcceptanceRadius;
    const bool bIsStopped = Speed < 5.f;

    if (bIsCloseEnough && bIsStopped)
    {
        if (bIsMoving)
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Default);
            Boss->PlayIdleAnimation();
            bIsMoving = false;

            if (PlayerCharacter)
            {
                PlayerCharacter->SetHP(-10.f); // 플레이어 HP 감소
            }
        }
    }
    else if (!bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Boss->PlayWalkAnimation();
        bIsMoving = true;
    }
}
