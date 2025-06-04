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

    class AUNPJCharacter* PlayerCharacter = nullptr; // JH_플레이어 캐릭터 

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

    bool bBurrowAttackDamage = false;

    void StartBurrow();
    void ExecuteAmbush();

    UFUNCTION()
    void OnAmbushOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
