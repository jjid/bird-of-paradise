#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BasicBoss.generated.h"

UCLASS()
class UNPJ_API ABasicBoss : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    class AUNPJCharacter* PlayerCharacter = nullptr;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 200.f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;
};
