#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EControllerLongDistance.generated.h"

UCLASS()
class UNPJ_API AEControllerLongDistance : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 2000.f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
    bool bIsCharging = false; 

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeTriggerDistance = 3000.f; // 돌진 개시 거리

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChargeSpeed = 1200.f; // 돌진 속도

    UPROPERTY(EditAnywhere, Category = "AI")
    float WalkSpeed = 300.f; // 기본 걷기 속도
};
