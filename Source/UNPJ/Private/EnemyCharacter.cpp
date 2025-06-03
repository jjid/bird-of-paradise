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
        GetMesh()->PlayAnimation(ThrowAnimation, false); // false = 1ȸ ���
        CurrentAnim = nullptr; // ������ ���� �ٽ� Idle üũ�� �� �ֵ��� �ʱ�ȭ
    }
}

void AEnemyCharacter::PlayJumpAttackAnimation()
{
    if (JumpAttackMontage)
    {
        PlayAnimMontage(JumpAttackMontage);
    }
}