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

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 300.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float DamageCooldown = 1.0f;

private:
    UPROPERTY()
    APawn* PlayerPawn = nullptr;

    UPROPERTY()
    class AUNPJCharacter* PlayerCharacter = nullptr;

    bool bIsMoving = false;
    bool bDidAttack = false;

    FTimerHandle DamageCooldownHandle;

    UFUNCTION()
    void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void ResetAttack();
};
