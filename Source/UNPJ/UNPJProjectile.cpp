// Copyright Epic Games, Inc. All Rights Reserved.

#include "UNPJProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h" // 파티클 시스템 컴포넌트 추가

AUNPJProjectile::AUNPJProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AUNPJProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// 총알 메시 root 아래로 설정
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	// 총알은 3초까지만 유지후 삭제
	InitialLifeSpan = 3.0f;
}

void AUNPJProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && !( OtherActor->ActorHasTag("Player") ) ) // 내 캐릭터는 무시
	{
		Destroy(); // 일단 총알이 뭔가랑 부딪히기만 하면 총알 삭제
		UE_LOG(LogTemp, Warning, TEXT("총알이랑 부딪힌 액터 이름 : %s"), *OtherActor->GetName());
	}
}