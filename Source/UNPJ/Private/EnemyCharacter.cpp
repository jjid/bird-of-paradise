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
    // ��Ʈ�ѷ��� �ִϸ��̼� ���� �� ���⼱ �� ��
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