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

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackRange = 1000.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float BurrowDelay = 1.5f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackCooldown = 5.0f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
    bool bIsBurrowed = false;
    bool bIsInCooldown = false;

    FTimerHandle BurrowTimerHandle;
    FTimerHandle CooldownTimerHandle;

    void StartBurrow();
    void ExecuteAmbush();
};
