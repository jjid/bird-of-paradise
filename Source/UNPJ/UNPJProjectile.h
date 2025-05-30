// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UNPJProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AUNPJProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile, meta=(AllowPrivateAccess="true"))
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// 총알 스태틱 메시
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* BulletMesh;

public:
	AUNPJProjectile();

	/** 총알 속도 (에디터/블루프린트에서 변경 가능) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float BulletSpeed = 3000.f;

	// 총알 속도 바꾸는 함수
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetBulletSpeed(float AddSpeed);

	class AUNPJCharacter* OwnerCharacter = nullptr; // 총알을 발사한 캐릭터
	
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

