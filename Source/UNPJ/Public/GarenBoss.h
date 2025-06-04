#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GarenBoss.generated.h"

UCLASS()
class UNPJ_API AGarenBoss : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 200.f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
    bool bIsCharging = false;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeTriggerDistance = 20000.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeSpeed = 3000.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float WalkSpeed = 400.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeCooldownTime = 2.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float SpinSpeed = 720.0f;

    FTimerHandle ChargeCooldownTimerHandle;
    bool bCanCharge = true;

    UFUNCTION()
    void ResetCharge();
};
