#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class UNPJ_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    virtual void Tick(float DeltaTime) override;

    void PlayWalkAnimation();         // 걷는 애니메이션
    void PlayIdleAnimation();         // 대기 애니메이션
    void PlayThrowAnimation();        // 투척 애니메이션
    void PlaySmashAttackAnimation();  // 순간이동 공격 애니메이션

protected:
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* WalkAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* IdleAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* ThrowAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* SmashAttackAnimation;

private:
    UAnimationAsset* CurrentAnim = nullptr;
};
