#include "EController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "EController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.h"
#include "AIController.h"            
#include "GameFramework/PlayerController.h"
#include "NavigationSystem.h" 



void AEController::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn && GetPawn())
    {
        SetFocus(PlayerPawn);
        MoveToActor(PlayerPawn, AcceptanceRadius-100, true, true, true, nullptr, false);

        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
        if (Enemy)
        {
            Enemy->PlayWalkAnimation();
        }

        bIsMoving = true;
    }
}


void AEController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
    float Speed = ControlledPawn->GetVelocity().Size();

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ControlledPawn);
    if (!Enemy) return;

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

            UE_LOG(LogTemp, Warning, TEXT("🛑 멈춤 + Idle 전환: 거리 %.1f, 속도 %.1f"), Distance, Speed);
        }
    }
    else if (!bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius-100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Enemy->PlayWalkAnimation();
        bIsMoving = true;

        UE_LOG(LogTemp, Warning, TEXT("🏃 다시 추적 시작: 거리 %.1f, 속도 %.1f"), Distance, Speed);
    }
}