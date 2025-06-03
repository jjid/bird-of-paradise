#include "UNPJGrenade.h"
#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h" // 이펙트 스폰에 필요
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"// 사발면 깃허브 코파일럿 멍청한 자식
#include "EnemyCharacter.h" // 적 캐릭터 클래스 포함
#include "UNPJCharacter.h" // 플레이어 캐릭터 클래스 포함

AUNPJGrenade::AUNPJGrenade()
{
    // 콜리전(캡슐) 컴포넌트 생성 및 설정
    CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    CollisionComp->InitCapsuleSize(15.0f, 30.0f); // 반지름, 하프-높이
    CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    RootComponent = CollisionComp;

    // 스태틱 메시 컴포넌트 생성 및 설정
    GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
    GrenadeMesh->SetupAttachment(RootComponent); // 콜리전 컴포넌트에 부착

    // 무브먼트 컴포넌트 생성 및 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = GrenadeSpeed;
    ProjectileMovement->MaxSpeed = GrenadeSpeed;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true; // 수류탄은 튕길 수 있음

    // 수류탄은 5초 후 자동 삭제
    InitialLifeSpan = GrenadeDelay;

    // DefaultObject의 ExplosionRadius 값을 복사-> 다른 코드에서 값 변경한거 적용 시키는 코드!!!!!!
    if (!HasAnyFlags(RF_ClassDefaultObject))
    {
        AUNPJGrenade* CDO = Cast<AUNPJGrenade>(GetClass()->GetDefaultObject());
        if (CDO)
        {
            ExplosionRadius = CDO->ExplosionRadius;
        }
    }
}

void AUNPJGrenade::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // 폭발 함수 호출
    Explode();
}

void AUNPJGrenade::Explode()
{
    //UE_LOG(LogTemp, Warning, TEXT("수류탄 폭발!"));

    // 파티클 생성
    if (GrenadeExplosionParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(),
        GrenadeExplosionParticle,
        GetActorLocation(),
        GetActorRotation()
        );
        
    }
    // 폭발 범위 디버그(빨간색 원)
    //UE_LOG(LogTemp, Warning, TEXT("폭발 범위 : %f"), ExplosionRadius);
    //DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 32, FColor::Red, false, 2.0f);

    // 폭발 범위 내 액터 감지
    TArray<FOverlapResult> Overlaps; // "Engine/OverlapResult.h" 이거 넣어줘야함. 코파일럿 이거 모름
    FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionRadius);

    bool bHit = GetWorld()->OverlapMultiByChannel(
        Overlaps,             // 감지된 결과(액터, 컴포넌트 등)가 저장될 배열
        GetActorLocation(),   // 구의 중심(여기서는 수류탄 위치)
        FQuat::Identity,      // 회전(구는 회전이 의미 없음, 기본값)
        ECC_Pawn,             // 감지할 오브젝트 채널 (여기선 Pawn, 즉 캐릭터)
        Sphere                // 구의 크기(반경)
    );

    if (bHit)
    {
        for (auto& Result : Overlaps)
        {
            AActor* OtherActor = Result.GetActor();
            if (OtherActor && OtherActor != this && OtherActor->IsA(AEnemyCharacter::StaticClass() ) )
            {
                // 여기서 데미지 적용 등 원하는 처리
                //UE_LOG(LogTemp, Warning, TEXT("폭발 대미지 받는 액터 : %s"), *OtherActor->GetName());
                OtherActor->Destroy();

               // 내 캐릭터의 경험치 증가 사발면 이거 왜 겟 오우너로는 안됨?
                if ( OwnerCharacter && OwnerCharacter->IsA(AUNPJCharacter::StaticClass()) )
                {
                    OwnerCharacter->SetExp(10.f);
                }
            }
        }
    }

    Destroy();
}

void AUNPJGrenade::SetGrenadeSpeed(float AddSpeed)

{
    GrenadeSpeed += AddSpeed; // 기존 속도에 AddSpeed 더하기
    GrenadeSpeed = FMath::Max(0.f, GrenadeSpeed); // 최소 0으로 제한
    //UE_LOG(LogTemp, Warning, TEXT("수류탄 속도 : %f"), GrenadeSpeed);
    ProjectileMovement->InitialSpeed = GrenadeSpeed;
    ProjectileMovement->MaxSpeed = GrenadeSpeed;
}

void AUNPJGrenade::SetExplosionRadius(float AddRadius)
{
    ExplosionRadius += AddRadius; // 기존 반경에 AddRadius 더하기
    ExplosionRadius = FMath::Max(0.f, ExplosionRadius); // 최소 0으로 제한
    //UE_LOG(LogTemp, Warning, TEXT("수류탄 폭발 반경 : %f"), ExplosionRadius);
}

void AUNPJGrenade::SetGrenadeDelay(float AddDelay)
{
    GrenadeDelay += AddDelay; // 기존 지연 시간에 AddDelay 더하기
    GrenadeDelay = FMath::Max(0.f, GrenadeDelay); // 최소 0으로 제한
    //UE_LOG(LogTemp, Warning, TEXT("수류탄 지연 시간 : %f"), GrenadeDelay);
}