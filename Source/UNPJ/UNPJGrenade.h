// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UNPJGrenade.generated.h"

class UCapsuleComponent;
class UProjectileMovementComponent;

UCLASS()
class UNPJ_API AUNPJGrenade : public AActor
{
	GENERATED_BODY()
	
	/** Capsule collision component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile, meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// 수루탄
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* GrenadeMesh;

	//수류탄 터지는 파티클 - 케스케이드
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* GrenadeExplosionParticle;

public:	
	// Sets default values for this actor's properties
	AUNPJGrenade();

	// 수류탄이 삭제되는 순간 엔드플레이 -> 폭발 함수 실행
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    void Explode();

	// 수류탄 정보 - 던지는 속도, 터지는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float GrenadeSpeed = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float GrenadeDelay = 1.5f; // 수류탄은 3초 후에 삭제됨


	// 수류탄 폭발 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ExplosionRadius = 300.f; // 원하는 폭발 반경 값으로 설정

	// 수류탄 폭발 범위 바꾸는 함수
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetExplosionRadius(float AddRadius);

	// 수류탄 터지는 시간 바꾸는 함수
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetGrenadeDelay(float AddDelay);

	// 수류탄 속도 바꾸는 함수
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetGrenadeSpeed(float AddSpeed);

	// 수류탄 던지는 캐릭터
	class AUNPJCharacter* OwnerCharacter = nullptr;

};
