#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"

#include "EnemyCharacter.generated.h" 

UCLASS()
class UNPJ_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    virtual void Tick(float DeltaTime) override;

    void PlayWalkAnimation();     // 걷는 애니메이션 재생
    void PlayIdleAnimation();     // 도착 애니메이션 재생
    void PlayThrowAnimation();    // 던지는 애니메이션 재생
    void PlayJumpAttackAnimation(); // 점프 공격 애니메이션 재생


protected:
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* WalkAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* IdleAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* ThrowAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation") 
    UAnimMontage* JumpAttackMontage;


private:
    UAnimationAsset* CurrentAnim = nullptr;
};
