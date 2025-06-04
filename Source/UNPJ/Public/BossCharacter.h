#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossCharacter.generated.h"

UCLASS()
class UNPJ_API ABossCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABossCharacter();

    virtual void Tick(float DeltaTime) override;

    // 체력 시스템
    void TakeDamage(float DamageAmount);
    bool IsDead() const;

    // 애니메이션
    void PlayWalkAnimation();
    void PlayIdleAnimation();
    void PlaySmashAttackAnimation();
    void PlayThrowAnimation(); 

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Boss Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Boss Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* WalkAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* IdleAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* SmashAttackAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* ThrowAnimation;

private:
    UAnimationAsset* CurrentAnim = nullptr;

    void Die();
};
