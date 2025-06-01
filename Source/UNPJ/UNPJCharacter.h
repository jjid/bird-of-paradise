// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Blueprint/UserWidget.h" // 위젯 사용 헤더
#include "CharacterWidget.h"      // 캐릭터 위젯 헤더
#include "UNPJProjectile.h" // 안 넣어도 되는데 일단 넣어줌
#include "UNPJGrenade.h" // 왜인지 모르겠는데 발사체는 어디서 이미 선언 되있고 수류탄은 안되있다고 함?
#include "UNPJCharacter.generated.h" // 반드시 마지막에 위치해야 함

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UStaticMeshComponent;

// 캐릭터 상태 열거형 변수
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Idle        UMETA(DisplayName = "Idle"), // 평상시 움직이는 상태
    Firing      UMETA(DisplayName = "Firing"), // 총 쏘는 상태
	Jumping     UMETA(DisplayName = "Jumping"),// 점프
	UsingSkiill UMETA(DisplayName = "UsingSkill"), // 스킬 사용
    Reloading   UMETA(DisplayName = "Reloading"), // 재장전
    Dead        UMETA(DisplayName = "Dead") // 엄
};

UCLASS(config=Game)
class AUNPJCharacter : public ACharacter
{
    GENERATED_BODY()

    // ====== 컴포넌트 ======//////////////////////////////////////////////////////////
    /** 1인칭 팔 메시 (본인만 볼 수 있음) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= C_Mesh, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* Mesh1P;

    /** 1인칭 카메라 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

    /** 1인칭 카메라 하위에 부착되는 스태틱 메시(총) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Mesh, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* SM_Gun;

    /** 총알이 나가는 위치 컴포넌트 (총 아래에 부착) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Mesh, meta = (AllowPrivateAccess = "true"))
    USceneComponent* FireLocation;

    // 수류탄 생성 위치 - 위치는 카메라 아래에 부착
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Mesh, meta = (AllowPrivateAccess = "true"))
    USceneComponent* GrenadeLocation;

    // ====== 입력 관련 ======////////////////////////////////////////////////
    /** 입력 매핑 컨텍스트 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    /** 점프 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= C_Input, meta=(AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    /** 이동 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= C_Input, meta=(AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    /** 시야 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

	/** 재장전 입력 액션 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

    /** 총 발사 입력 액션 (마우스 왼쪽 클릭) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* FireAction;

    // 수류탄 투척 입력 액션 G
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* GrenadeAction;

    // ====== 이펙트/액터 관련 ======///////////////////////////////////////////
    /** 총 발사 시 나오는 파티클 이펙트 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Effects, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* FireEffect;

    /** 총알 액터 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Actor, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class AUNPJProjectile> ProjectileClass;

    /** 수류탄 액터 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Actor, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class AUNPJGrenade> GrenadeClass;

    // ====== UI 관련 ======
    /** 캐릭터 위젯 클래스 (블루프린트에서 할당) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_UI, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UCharacterWidget> CharacterWidgetClass;

    /** 캐릭터 위젯 인스턴스 */
    UPROPERTY()
    UCharacterWidget* CharacterWidget;

	/** 캐릭터 상태 (Idle, Firing, Jumping 등) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_State", meta = (AllowPrivateAccess = "true"))
    ECharacterState CharacterState = ECharacterState::Idle;

    // ====== 캐릭터 스탯 ======////////////////////////////////////////////
    /** 최대 체력 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float MaxHP = 100.f;

    /** 현재 체력 */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float CurrentHP = 100.f;

    /** 최대 경험치 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float MaxExp = 100.f;

    /** 현재 경험치 */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float CurrentExp = 0.f;

    /** 현재 총알 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 CurrentBullet = 6;

    /** 최대 총알 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 MaxBullet = 6;

    /** 현재 수류탄 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 CurrentGrenade = 2;

    /** 최대 수류탄 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 MaxGrenade = 2;

    // 게임 매니저 역할!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // UI에 표시할 타이머
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float TimerValue = 0.f;
    // 한 라운드당 
    // 타이머 핸들러
    FTimerHandle UI_TimerHandle;
    // UI에 표시할 라운드
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 RoundValue = 1;
    
	/** 재장전 애니메이션 *///////////////////////////////////////////////////////
    float ReloadElapsed = 0.f;
    float ReloadDuration = 0.5f; // 재장전 시간
    FTimerHandle ReloadTimerHandle;

	void ReloadInterpStep(); // 재장전 애니메이션 회전값 보간
	FRotator GunIdleRot; // 총의 기본(초기) 회전값
	/** 총 쏘는 애니메이션 */
    float FireElapsed = 0.f;
    float FireDuration = 0.35f; // 총 쏘는 시간
	float FireDegree = 40.f; // 총 쏠떄 튀는 각도( 0.01 곱하기 틱 시간 )
	FTimerHandle FireTimerHandle;

	void FireInterpStep(); // 총쏘는 애니메이션 회전값 위로 보간
    void ReturnGunToIdle(float DeltaTime); // 아래로 보간.

protected:
    // ====== 입력 함수 ======////////////////////////////////////////
    /** 이동 입력 처리 */
    void Move(const FInputActionValue& Value);

    /** 시야 입력 처리 */
    void Look(const FInputActionValue& Value);

    /** 총 발사 함수 */
    void Fire();

    /** 재장전 함수 */
    void Reload();

    /** 수류탄 투척 함수 */
    void ThrowGrenade();

    // ====== 엔진 오버라이드 ======
    virtual void NotifyControllerChanged() override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    virtual void BeginPlay() override;
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void Tick(float DeltaTime) override;

public:
    AUNPJCharacter();
    // ====== Getter ======
    /** 1인칭 팔 메시 반환 */
    USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
    /** 1인칭 카메라 반환 */
    UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


    // 게임 진행중에 이벤트 발생시 스탯 변경하는 함수//////////////////////////////
    // ====== 스탯 설정 함수 ======
    /** 체력 설정 */
    UFUNCTION(BlueprintCallable, Category = "C_Function")			
    void SetHP(float AddHp);

    /** 경험치 설정 */
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetExp(float AddExp);

    /** 총알 개수 설정 후 UI 표시 */
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetBullet(int32 NewBullet);

    /** 수류탄 개수 설정 후 UI 표시 */
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetGrenade(int32 AddGrenade)
    {
        int32 NewGrenade = CurrentGrenade + AddGrenade;
        // 최소 수류탄 개수는 0개
        CurrentGrenade = FMath::Max(0, NewGrenade);
        if (CharacterWidget)
        {
             FString GrenadeText = FString::Printf(TEXT("%d / %d"), CurrentGrenade, MaxGrenade);
             CharacterWidget->GrenadeState->SetText(FText::FromString(GrenadeText));
        }
    }

	// 재장전 시간 설정 함수
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetReloadDuration(float AddDuration)
    {
        float NewDuration = ReloadDuration + AddDuration;
        // 최소 재장전 시간은 0.1초
        ReloadDuration = FMath::Max(0.1f, NewDuration);
    }
    // 총알 최대 갯수 변경 함수
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetMaxBullet(int32 AddBullet)
    {
        int32 NewBullet = MaxBullet + AddBullet;
        // 최소 총알 개수는 1개
        MaxBullet = FMath::Max(1, NewBullet);
    }
    // 총알의 속도 변경 함수
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetBulletSpeed(float AddSpeed)
    {
        if (ProjectileClass)
        {
            AUNPJProjectile* DefaultProjectile = Cast<AUNPJProjectile>(ProjectileClass->GetDefaultObject());
            if (DefaultProjectile)
            {
                DefaultProjectile->SetBulletSpeed(AddSpeed);
            }
        }
    }
    // 수류탄 폭발 범위 변경 함수 -- 
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetGrenadeRadius(float AddRange)
    {
        if (GrenadeClass)
        {
            AUNPJGrenade* DefaultGrenade = Cast<AUNPJGrenade>(GrenadeClass->GetDefaultObject());
            if (DefaultGrenade)
            {
                DefaultGrenade->SetExplosionRadius(AddRange);
            }
        }
    }
};

