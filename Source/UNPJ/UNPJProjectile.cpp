// Copyright Epic Games, Inc. All Rights Reserved.

#include "UNPJProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyCharacter.h"
#include "BossCharacter.h"
#include "UNPJCharacter.h"

AUNPJProjectile::AUNPJProjectile()
{
	// 충돌 컴포넌트 생성 및 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AUNPJProjectile::OnHit);
	RootComponent = CollisionComp;

	// 총알 메시
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);

	// 이동 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = BulletSpeed;
	ProjectileMovement->MaxSpeed = BulletSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;

	// 총알 수명 설정
	InitialLifeSpan = 3.0f;
}

void AUNPJProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || !OtherComp) return;
	if (OtherActor->ActorHasTag("Player")) return;

	if (ABossCharacter* Boss = Cast<ABossCharacter>(OtherActor))
	{
		Boss->TakeDamage(25.f); // 보스는 체력 기반 데미지
	}
	else if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor))
	{
		Enemy->Destroy();

		if (OwnerCharacter && OwnerCharacter->IsA(AUNPJCharacter::StaticClass()))
		{
			OwnerCharacter->SetExp(10.f); // 경험치 부여
		}
	}

	Destroy(); // 총알 제거
}

void AUNPJProjectile::SetBulletSpeed(float AddSpeed)
{
	float NewSpeed = FMath::Clamp(BulletSpeed + AddSpeed, 0.f, 10000.f);
	BulletSpeed = NewSpeed;
	ProjectileMovement->InitialSpeed = BulletSpeed;
	ProjectileMovement->MaxSpeed = BulletSpeed;
}
