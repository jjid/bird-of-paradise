#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "JumpController.generated.h"

UCLASS(Blueprintable)
class UNPJ_API AJumpController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 200.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackTriggerDistance = 300.f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
    bool bIsJumpAttacking = false;

    FTimerHandle JumpAttackCooldownHandle;

    void OnJumpAttackFinished(); // ���� ���� ���� �� ȣ��
};