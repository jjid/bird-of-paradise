#include "TeleportController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "UNPJ/UNPJCharacter.h"
#include "Components/CapsuleComponent.h"

void ATeleportController::BeginPlay()
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
        }

        bIsMoving = true;

        // 오버랩 함수 바인딩 
        UCapsuleComponent* Capsule = Enemy->GetCapsuleComponent();
        if (Capsule)
        {
            Capsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATeleportController::OnJumpAttackOverlap);
        }
    }
    PlayerCharacter = Cast<AUNPJCharacter>(PlayerPawn); // JH_ 캐릭터 가져오기
}

void ATeleportController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerPawn || !GetPawn()) return;

    APawn* ControlledPawn = GetPawn();
    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledPawn->GetActorLocation());
    float Speed = ControlledPawn->GetVelocity().Size();

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ControlledPawn);
    if (!Enemy) return;

    const bool bIsCloseEnough = Distance <= AcceptanceRadius;
    const bool bInJumpRange = Distance <= JumpAttackDistance;
    const bool bIsStopped = Speed < 5.f;

    if (bInJumpRange && !bIsJumpAttacking)
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Default);
        bIsJumpAttacking = true;

        PerformJumpAttack();

        GetWorld()->GetTimerManager().SetTimer(JumpAttackCooldownHandle, [this]()
            {
                bIsJumpAttacking = false;
                UE_LOG(LogTemp, Warning, TEXT("Jump cooldown ended"));

            }, JumpAttackCooldown, false);

        return;
    }

    if (bIsCloseEnough && bIsStopped)
    {
        if (bIsMoving)
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Default);
            Enemy->PlayIdleAnimation();
            bIsMoving = false;
        }
    }
    else if (!bIsCloseEnough && !bIsJumpAttacking)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Enemy->PlayWalkAnimation();
        bIsMoving = true;
    }
}

void ATeleportController::PerformJumpAttack()
{
    ACharacter* ControlledChar = Cast<ACharacter>(GetPawn());
    if (!ControlledChar || !PlayerPawn) return;

    bJumpAttackDamage = false; // 점프 어택 시작 시 초기화

    FVector StartLoc = ControlledChar->GetActorLocation();
    FVector TargetLoc = PlayerPawn->GetActorLocation();

    FVector Dir = (TargetLoc - StartLoc).GetSafeNormal2D();
    FVector LaunchVelocity = Dir * 1000.f + FVector(0.f, 0.f, 800.f); // �� + �� ����

    ControlledChar->SetActorRotation(Dir.Rotation());
    ControlledChar->LaunchCharacter(LaunchVelocity, true, true);

    UE_LOG(LogTemp, Warning, TEXT("Forced jump velocity: %s"), *LaunchVelocity.ToString());

    GetWorld()->GetTimerManager().SetTimer(JumpAttackCooldownHandle, [this]()
        {
            AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
            if (Enemy)
            {
                Enemy->PlaySmashAttackAnimation();
            }

            bIsJumpAttacking = false;

            if (PlayerPawn && GetPawn())
            {
                MoveToActor(PlayerPawn, AcceptanceRadius - 100, true, true, true, nullptr, false);
                SetFocus(PlayerPawn);
            }

        }, 1.0f, false);
}

void ATeleportController::OnJumpAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
    if (!bJumpAttackDamage && EnemyChar && EnemyChar->GetCharacterMovement()->IsFalling() && OtherActor == PlayerPawn)
    {
        bJumpAttackDamage = true; // 한 번만 대미지
        if (PlayerCharacter)
        {
            PlayerCharacter->SetHP(-10.f); // 플레이어 HP 감소
        }
        //UE_LOG(LogTemp, Warning, TEXT("공중(낙하)에서 오버랩 감지! 대미지 1회 적용"));
    }
}
