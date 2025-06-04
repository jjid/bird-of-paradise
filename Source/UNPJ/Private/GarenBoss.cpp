#include "GarenBoss.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BossCharacter.h"
#include "UNPJ/UNPJCharacter.h"
#include "Components/CapsuleComponent.h"

void AGarenBoss::ResetCharge()
{
    bCanCharge = true;
    bAttackDamage = true; // 다음 돌진에서 다시 데미지 가능
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

            UCapsuleComponent* Capsule = Boss->GetCapsuleComponent();
            if (Capsule)
            {
                Capsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGarenBoss::OnAttackOverlap);
            }
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
        bAttackDamage = false; // 돌진 중 데미지 한 번만

        StopMovement();
        ClearFocus(EAIFocusPriority::Default);

        FVector BossLocation = ControlledPawn->GetActorLocation();
        FVector PlayerLocation = PlayerPawn->GetActorLocation();
        FVector FlatTarget = FVector(PlayerLocation.X, PlayerLocation.Y, BossLocation.Z); // Z 고정

        FVector Direction = (FlatTarget - BossLocation).GetSafeNormal();
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

void AGarenBoss::OnAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 돌진 중 & 아직 공격 안 했고, 플레이어와 충돌 시
    if (!bAttackDamage && OtherActor == PlayerPawn && PlayerCharacter)
    {
        bAttackDamage = true;
        PlayerCharacter->SetHP(-20.f); // 원하는 만큼의 데미지
    }
}
