// EController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EController.generated.h"

UCLASS()
class UNPJ_API AEController : public AAIController
{
    GENERATED_BODY()

public:
    AEController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "AI")
    float StopDistance = 300.f;

    UPROPERTY()
    APawn* PlayerPawn = nullptr;
};
