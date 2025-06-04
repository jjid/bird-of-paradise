#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "JumpBoss.generated.h"

UCLASS()
class UNPJ_API AJumpBoss : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    class AUNPJCharacter* PlayerCharacter = nullptr; // JH_플레이어 캐릭터 

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 1200.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float JumpAttackDistance = 1200.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float JumpAttackCooldown = 3.0f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
    bool bIsJumpAttacking = false;

    FTimerHandle JumpAttackCooldownHandle;

    void PerformJumpAttack();

    bool bJumpAttackDamage = false;

    UFUNCTION()
    void OnJumpAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
