#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
        GetMesh()->PlayAnimation(ThrowAnimation, false);
        CurrentAnim = nullptr;
    }
}
void AEnemyCharacter::PlaySmashAttackAnimation()
{
    if (SmashAttackAnimation && CurrentAnim != SmashAttackAnimation)
    {
        GetMesh()->PlayAnimation(SmashAttackAnimation, false); // 한 번만 재생
        CurrentAnim = nullptr; // 공격 후 Idle 등으로 돌아갈 수 있도록 초기화
    }
}