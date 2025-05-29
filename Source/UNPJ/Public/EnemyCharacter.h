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

    void PlayWalkAnimation();     // 걷는 애니메이션 재생
    void PlayIdleAnimation();     // 도착 애니메이션 재생
    void PlayThrowAnimation();    // 던지는 애니메이션 재생

protected:
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* WalkAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* IdleAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimationAsset* ThrowAnimation;

private:
    UAnimationAsset* CurrentAnim = nullptr;
};
