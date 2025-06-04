// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Blueprint/UserWidget.h" // 위젯 사용 헤더
#include "CharacterWidget.h"      // 캐릭터 위젯 헤더
#include "DeadWidget.h"          // 캐릭터 사망시 위젯 헤더
#include "AbilitySelectWidget.h" // 스킬 선택 위젯 헤더
#include "UNPJProjectile.h" // 안 넣어도 되는데 일단 넣어줌
#include "UNPJGrenade.h" // 왜인지 모르겠는데 발사체는 어디서 이미 선언 되있고 수류탄은 안되있다고 함?
#include "GameFramework/CharacterMovementComponent.h"
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
    Dashing     UMETA(DisplayName = "Dashing"), // 대쉬
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

    // 대쉬 입력 왼쪽 쉬프트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DashAction;

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

    // ====== UI 관련 ======////////////////////////////////////////////////////////
    /** 캐릭터 위젯 클래스 (블루프린트에서 할당) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_UI, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UCharacterWidget> CharacterWidgetClass;

    /** 캐릭터 위젯 인스턴스 */
    UPROPERTY()
    UCharacterWidget* CharacterWidget;

    // 캐릭터 사망시 위젯 클래스( 블루 프린트에서 할당 )
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_UI, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UDeadWidget> DeathWidgetClass;

    /** 캐릭터 사망시 위젯 인스턴스 */
    UPROPERTY()
    UDeadWidget* DeadWidget;

    // 스킬 선택 위젯 클래스 (블루 프린트에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_UI, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UAbilitySelectWidget> AbilitySelectWidgetClass;

    /** 스킬 선택 위젯 인스턴스 */
    UPROPERTY()
    UAbilitySelectWidget* AbilitySelectWidget;

	/** 캐릭터 상태 (Idle, Firing, Jumping 등) */////////////////////////////////////////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_State", meta = (AllowPrivateAccess = "true"))
    ECharacterState CharacterState = ECharacterState::Idle;

    // ====== 캐릭터 스탯 ======////////////////////////////////////////////
    /** 최대 체력 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float MaxHP = 100.f;

    /** 현재 체력 */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float CurrentHP = 100.f;

    // 레벨업시 증가되는 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float LevelUpHPIncrease = 20.f; // 레벨업시 증가되는 체력 (예: 20)

    /** 최대 경험치 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float MaxExp = 100.f;

    /** 현재 경험치 */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float CurrentExp = 0.f;

    // 레벨업시 증가되는 경험치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float LevelUpExpIncrease = 20.f; // 레벨업시 증가되는 경험치 (예: 20)

    /** 현재 총알 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 CurrentBullet = 6;

    /** 최대 총알 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 MaxBullet = 6;

    // 수류탄 쿨타임 관련 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float GrenadeCooldown = 5.0f; // 수류탄 쿨타임 (초)

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    bool bCanThrowGrenade = true; // 수류탄 던질 수 있는지 여부

    FTimerHandle GrenadeCooldownTimerHandle; // 수류탄 쿨타임 타이머
    float GrenadeCooldownElapsed = 0.f; // 수류탄 쿨타임 경과 시간
    void ResetGrenadeCooldown()
    {
        bCanThrowGrenade = true; // 쿨타임이 끝나면 수류탄 던질 수 있게 설정
        //UE_LOG(LogTemp, Warning, TEXT("수류탄 쿨타임 종료!"));
        GrenadeCooldownElapsed = 0.f;
        if (CharacterWidget && CharacterWidget->GrenadeSkillMask) CharacterWidget->GrenadeSkillMask->SetPercent(0.f);
    }

    // 대쉬(고속 이동) 관련 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float DashCooldown = 3.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    bool bCanDash = true;

    FTimerHandle DashCooldownTimerHandle;
    float DashCooldownElapsed = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float DashDuration = 0.2f; // 대쉬 지속 시간 (초)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float GroundDashSpeed = 2000.f; // 땅에서 대쉬 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float AirDashSpeed = 2000.f; // 공중에서 대쉬 속도

    FVector2D LastMovementInputVector;
    float LastMovementInputTime;
    const float StaleInputThreshold = 0.2f;
    float OriginalGroundFriction;
    FTimerHandle DashDurationTimerHandle;

    void ResetDashCooldown()
    {
        bCanDash = true;
        //UE_LOG(LogTemp, Warning, TEXT("대쉬 쿨타임 종료!"));
        DashCooldownElapsed = 0.f;
        if (CharacterWidget && CharacterWidget->DashSkillMask) CharacterWidget->DashSkillMask->SetPercent(0.f);
        CharacterState = ECharacterState::Idle;
    }

    // 게임 매니저 역할!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // UI에 표시할 타이머
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    float TimerValue = 0.f;
    float RoundChangeTime = 60.f; // 라운드 변경 시간 (초 단위)
    // 한 라운드당 
    // 타이머 핸들러
    FTimerHandle UI_TimerHandle;
    // UI에 표시할 라운드
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C_Stat", meta = (AllowPrivateAccess = "true"))
    int32 RoundValue = 1;
    // 라운드 변경 함수
    void ChangeRound( int32 ComputedRound )
    {
        RoundValue = ComputedRound;
        FString RoundText = FString::Printf(TEXT("Round: %d"), RoundValue);
        CharacterWidget->RoundState->SetText(FText::FromString(RoundText));
    }
    
	/** 재장전 애니메이션 *///////////////////////////////////////////////////////////////////////////////////////
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

    /** 대쉬 함수 */
    void Dash();

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

    // 사운드 관련!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C_Sound")
    USoundBase* FireSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C_Sound")
    USoundBase* DeadSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C_Sound")
    USoundBase* LevelUpSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C_Sound")
    USoundBase* SkillSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C_Sound")
    USoundBase* DashSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C_Sound")
    USoundBase* ReloadSound;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C_Sound")
    USoundBase* OnHitSound;

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
  
    // 레벨업 했을때 선택지 선택시 실행되는 함수
    // 최대 체력 증가 0
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void IncreaseMaxHP()
    {
        MaxHP += LevelUpHPIncrease; // 레벨업시 증가되는 체력
        CurrentHP = MaxHP; // 현재 체력도 최대 체력으로 설정
        CharacterWidget->HealthBar->SetPercent(CurrentHP / MaxHP);
        FString HealthText = FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, MaxHP);
        CharacterWidget->HealthState->SetText(FText::FromString(HealthText));
    }

	// 재장전 시간 설정 함수 1
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetReloadDuration(float AddDuration)
    {
        float NewDuration = ReloadDuration + AddDuration;
        // 최소 재장전 시간은 0.1초
        ReloadDuration = FMath::Max(0.1f, NewDuration);
    }
    // 총알 최대 갯수 변경 함수 2
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetMaxBullet(int32 AddBullet)
    {
        int32 NewBullet = MaxBullet + AddBullet;
        // 최소 총알 개수는 1개
        MaxBullet = FMath::Max(1, NewBullet);
        CurrentBullet = MaxBullet; // 현재 총알 개수도 최대 총알 개수로 설정
        SetBullet(CurrentBullet); // UI 업데이트
    }
    // 총알의 속도 변경 함수 3
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
    // 수류탄 쿨타임 변경 함수 4
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetGrenadeCooldown(float AddCooldown)
    {
        GrenadeCooldown = FMath::Max(0.1f, GrenadeCooldown + AddCooldown); // 최소 쿨타임은 0.1초
    }
    // 대쉬 쿨타임 변경 함수 5
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void SetDashCooldown(float AddCooldown)
    {
        DashCooldown = FMath::Max(0.1f, DashCooldown + AddCooldown); // 최소 쿨타임은 0.1초
    }
    // 이동 속도 증가 함수 6
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void IncreaseMoveSpeed( )
    {
        if (GetCharacterMovement())
        {
            GetCharacterMovement()->MaxWalkSpeed += 30;
        }
    }
    // 총 쏘는 속도 증가 함수 7
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void DecreaseFireRate(float AddRate)
    {
        FireDuration = FMath::Max(0.1f, FireDuration + AddRate); // 최소 발사 시간은 0.1초
    }

    // 마지막으로 버튼 클릭으로 능력치 선택시 어빌리티 위젯 지우는 함수
    UFUNCTION(BlueprintCallable, Category = "C_Function")
    void RemoveAbilitySelectWidget();
};

