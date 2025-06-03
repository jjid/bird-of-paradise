#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TeleportController.generated.h"

UCLASS()
class UNPJ_API ATeleportController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 1200.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float JumpAttackDistance = 1200.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float JumpAttackCooldown = 3.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float JumpArc = 0.5f; // ������ ��� (0.0 = ����, 1.0 = ���� ����)

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
    bool bIsJumpAttacking = false;

    FTimerHandle JumpAttackCooldownHandle;

    void PerformJumpAttack();
};
