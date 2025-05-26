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

    void PlayWalkAnimation();     // �ȴ� �ִϸ��̼� ���
    void PlayIdleAnimation();     // ���� �ִϸ��̼� ���
    void PlayThrowAnimation();    // ������ �ִϸ��̼� ���

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
