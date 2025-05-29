#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DaggerThrow.generated.h"

UCLASS()
class UNPJ_API ADaggerThrow : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float ThrowTriggerDistance = 1500.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 200.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ThrowCooldownTime = 2.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    TSubclassOf<AActor> DaggerProjectileClass;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    FTimerHandle ThrowCooldownTimerHandle;
    bool bCanThrow = true;

    bool bIsThrowing = false;

    UFUNCTION()
    void ResetThrow();
};
