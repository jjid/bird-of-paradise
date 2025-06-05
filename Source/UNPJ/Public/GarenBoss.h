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

    UPROPERTY()
    class AUNPJCharacter* PlayerCharacter = nullptr;

    bool bIsMoving = false;
    bool bIsCharging = false;
    bool bCanCharge = true;
    bool bAttackDamage = true;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeTriggerDistance = 4000.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeSpeed = 4000.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float WalkSpeed = 400.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeCooldownTime = 4.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float SpinSpeed = 720.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeDurationTime = 4.0f;

    FTimerHandle ChargeCooldownTimerHandle;
    FTimerHandle ChargeEndTimerHandle;

    UFUNCTION()
    void ResetCharge();

    UFUNCTION()
    void EndCharge();
};
