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
#include "UNPJCharacter.h" // 플레이어 캐릭터 헤더



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
    PlayerCharacter = Cast<AUNPJCharacter>(PlayerPawn);
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

            PlayerCharacter->SetHP(-5.f);
        }
    }
    else if (!bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius-100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Enemy->PlayWalkAnimation();
        bIsMoving = true;

    }
}