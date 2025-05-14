// EController.cpp

#include "EController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.h" // ✅ 네가 만든 NPC 클래스 반드시 포함

AEController::AEController()
{
    PrimaryActorTick.bCanEverTick = true;
    UE_LOG(LogTemp, Warning, TEXT("📦 EController 생성됨"));

}

void AEController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("🔥 BeginPlay: AIController running"));

    // ✅ 명확하게 EnemyCharacter를 찾음 (절대 플레이어 캐릭터 아님)
    AEnemyCharacter* Found = Cast<AEnemyCharacter>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyCharacter::StaticClass()));

    if (Found && !Found->IsPlayerControlled())
    {
        UE_LOG(LogTemp, Warning, TEXT("👉 Possessing Enemy: %s"), *Found->GetName());
        Possess(Found);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ EnemyCharacter를 찾지 못했거나 이미 플레이어가 제어 중"));
    }

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AEController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());
    UE_LOG(LogTemp, Warning, TEXT("📏 거리: %f"), Distance);

    if (Distance > StopDistance)
    {
        MoveToActor(PlayerPawn);
    }
    else
    {
        StopMovement(); // ✔ 안전하게 작동함
    }
}
