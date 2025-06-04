#include "BurrowController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.h"
#include "TimerManager.h"
#include "UNPJ/UNPJCharacter.h"
#include "Components/CapsuleComponent.h"

void ABurrowController::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    PlayerCharacter = Cast<AUNPJCharacter>(PlayerPawn); // JH_ 캐릭터 가져오기

    // 오버랩 델리게이트 바인딩
    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (EnemyChar)
    {
        UCapsuleComponent* Capsule = EnemyChar->GetCapsuleComponent();
        if (Capsule)
        {
            Capsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABurrowController::OnAmbushOverlap);
        }
    }
}

void ABurrowController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!EnemyChar) return;

    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), EnemyChar->GetActorLocation());

    if (bIsBurrowed || bIsInCooldown)
        return;

    // �÷��̾ ��Ÿ� ���� ����
    if (Distance <= AttackRange)
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Default);
        StartBurrow();
        return;
    }

    // ���� ����
    if (!bIsMoving || Distance > AcceptanceRadius)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 50, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);

        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyChar);
        if (Enemy)
        {
            Enemy->PlayWalkAnimation();
        }

        bIsMoving = true;
    }
    else if (Distance <= AcceptanceRadius)
    {
        StopMovement();
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyChar);
        if (Enemy)
        {
            Enemy->PlayIdleAnimation();
        }

        bIsMoving = false;
    }
}

void ABurrowController::StartBurrow()
{
    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!EnemyChar) return;

    EnemyChar->SetActorHiddenInGame(true);
    EnemyChar->SetActorEnableCollision(false);
    bIsBurrowed = true;


    GetWorld()->GetTimerManager().SetTimer(BurrowTimerHandle, this, &ABurrowController::ExecuteAmbush, BurrowDelay, false);
}

void ABurrowController::ExecuteAmbush()
{
    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!EnemyChar || !PlayerPawn) return;

    FVector PlayerLoc = PlayerPawn->GetActorLocation();
    PlayerLoc.Z += 10.f;

    EnemyChar->SetActorLocation(PlayerLoc);
    EnemyChar->SetActorHiddenInGame(false);
    EnemyChar->SetActorEnableCollision(true);

    bBurrowAttackDamage = false;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyChar);
    if (Enemy)
    {
        Enemy->PlaySmashAttackAnimation();
    }

    bIsBurrowed = false;
    bIsInCooldown = true;


    GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
        {
            bIsInCooldown = false;
            UE_LOG(LogTemp, Warning, TEXT("��Ÿ�� ��"));
        }, AttackCooldown, false);
}

// 오버랩 콜백
void ABurrowController::OnAmbushOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bBurrowAttackDamage && OtherActor == PlayerPawn)
    {
        bBurrowAttackDamage = true;
        if (PlayerCharacter)
        {
            PlayerCharacter->SetHP(-20.f); // 
        }
        //UE_LOG(LogTemp, Warning, TEXT("땅에서 올라오는 동안 오버랩! 대미지 1회 적용"));
    }
}