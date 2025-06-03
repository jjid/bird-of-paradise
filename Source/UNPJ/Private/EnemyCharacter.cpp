#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyCharacter::PlayWalkAnimation()
{
    if (WalkAnimation && CurrentAnim != WalkAnimation)
    {
        GetMesh()->PlayAnimation(WalkAnimation, true);
        CurrentAnim = WalkAnimation;
    }
}

void AEnemyCharacter::PlayIdleAnimation()
{
    if (IdleAnimation && CurrentAnim != IdleAnimation)
    {
        GetMesh()->PlayAnimation(IdleAnimation, true);
        CurrentAnim = IdleAnimation;
    }
}

void AEnemyCharacter::PlayThrowAnimation()
{
    if (ThrowAnimation && CurrentAnim != ThrowAnimation)
    {
        GetMesh()->PlayAnimation(ThrowAnimation, false); // false = 1회 재생
        CurrentAnim = nullptr; // 던지고 나면 다시 Idle 체크할 수 있도록 초기화
    }
}

void AEnemyCharacter::PlayJumpAttackAnimation()
{
    if (JumpAttackMontage)
    {
        PlayAnimMontage(JumpAttackMontage);
    }
}