#include "BossCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABossCharacter::ABossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABossCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void ABossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABossCharacter::TakeDamage(float DamageAmount)
{
    if (IsDead()) return;

    CurrentHealth -= DamageAmount;
    UE_LOG(LogTemp, Warning, TEXT("Boss took %.1f damage, remaining HP: %.1f"), DamageAmount, CurrentHealth);

    if (CurrentHealth <= 0)
    {
        Die();
    }
}

bool ABossCharacter::IsDead() const
{
    return CurrentHealth <= 0;
}

void ABossCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Boss died"));
    Destroy(); // 죽는 애니메이션 후 파괴 가능
}

void ABossCharacter::PlayWalkAnimation()
{
    if (WalkAnimation && CurrentAnim != WalkAnimation)
    {
        GetMesh()->PlayAnimation(WalkAnimation, true);
        CurrentAnim = WalkAnimation;
    }
}

void ABossCharacter::PlayIdleAnimation()
{
    if (IdleAnimation && CurrentAnim != IdleAnimation)
    {
        GetMesh()->PlayAnimation(IdleAnimation, true);
        CurrentAnim = IdleAnimation;
    }
}

void ABossCharacter::PlaySmashAttackAnimation()
{
    if (SmashAttackAnimation && CurrentAnim != SmashAttackAnimation)
    {
        GetMesh()->PlayAnimation(SmashAttackAnimation, false);
        CurrentAnim = nullptr;
    }
}

void ABossCharacter::PlayThrowAnimation()
{
    if (ThrowAnimation && CurrentAnim != ThrowAnimation)
    {
        GetMesh()->PlayAnimation(ThrowAnimation, false);
        CurrentAnim = nullptr;
    }
}
