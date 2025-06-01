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
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    class AUNPJCharacter* PlayerCharacter = nullptr; // 플레이어 캐릭터

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 200.f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    bool bIsMoving = false;  
};