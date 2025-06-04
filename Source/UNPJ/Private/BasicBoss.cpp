#include "BasicBoss.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BossCharacter.h"
#include "UNPJ/UNPJCharacter.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"

void ABasicBoss::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    PlayerCharacter = Cast<AUNPJCharacter>(PlayerPawn);

    if (PlayerPawn && GetPawn())
    {
        SetFocus(PlayerPawn);
        MoveToActor(PlayerPawn, AcceptanceRadius-270 , true, true, true, nullptr, false);

        ABossCharacter* Boss = Cast<ABossCharacter>(GetPawn());
        if (Boss)
        {
            Boss->PlayWalkAnimation();

            UCapsuleComponent* Capsule = Boss->GetCapsuleComponent();
            if (Capsule)
            {
                Capsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABasicBoss::OnOverlapWithPlayer);
            }
        }

        bIsMoving = true;
    }
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
            // StopMovement()는 일부러 제거
            Boss->PlayIdleAnimation();
            bIsMoving = false;
        }
    }
    else if (!bIsCloseEnough)
    {
        MoveToActor(PlayerPawn, AcceptanceRadius - 270, true, true, true, nullptr, false);
        SetFocus(PlayerPawn);
        Boss->PlayWalkAnimation();
        bIsMoving = true;
    }
}

void ABasicBoss::OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bDidAttack && OtherActor == PlayerPawn)
    {
        bDidAttack = true;

        if (PlayerCharacter)
        {
            PlayerCharacter->SetHP(-10.f);
        }

        GetWorld()->GetTimerManager().SetTimer(DamageCooldownHandle, this, &ABasicBoss::ResetAttack, DamageCooldown, false);
    }
}

void ABasicBoss::ResetAttack()
{
    bDidAttack = false;
}
