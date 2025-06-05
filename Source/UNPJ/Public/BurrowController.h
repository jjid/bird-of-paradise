#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BurrowController.generated.h"

UCLASS()
class UNPJ_API ABurrowController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    class AUNPJCharacter* PlayerCharacter = nullptr;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackRange = 1000.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float BurrowDelay = 1.5f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackCooldown = 5.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AmbushHitRadius = 200.f; // 새로 추가: 데미지 판정 거리

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
    bool bIsBurrowed = false;
    bool bIsInCooldown = false;
    bool bBurrowAttackDamage = false;

    FTimerHandle BurrowTimerHandle;
    FTimerHandle CooldownTimerHandle;

    void StartBurrow();
    void ExecuteAmbush();
};
