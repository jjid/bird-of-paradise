// Copyright Epic Games, Inc. All Rights Reserved.

#include "UNPJCharacter.h"
#include "UNPJProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h" // 스태틱 메시 컴포넌트 추가
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h" // 이펙트 스폰에 필요
#include "UNPJProjectile.h" // 총알 액터에 필요
#include "UNPJProjectile.h" // 수류탄 액터에 필요
#include "Engine/LocalPlayer.h"
#include "DrawDebugHelpers.h" // 디버그 그리기용 헤더 추가
#include "Blueprint/UserWidget.h" // 위젯 사용을 위한 헤더 추가
#include "GameFramework/CharacterMovementComponent.h"

//////////////////////////////////////////////////////////////////////////
// AUNPJCharacter

AUNPJCharacter::AUNPJCharacter()
{
    // 캡슐 컴포넌트 크기 설정
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
        
    // 1인칭 카메라 컴포넌트 생성 및 부착
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // 카메라 위치 설정
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // 1인칭에서 보이는 스켈레탈 메시 컴포넌트 생성 및 카메라에 부착
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // 1인칭 카메라 하위에 스태틱 메시 컴포넌트(SM_Gun) 추가
    SM_Gun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Gun"));
    SM_Gun->SetupAttachment(FirstPersonCameraComponent);
    SM_Gun->SetRelativeLocation(FVector(30.f, 10.f, -20.f)); // 총 위치는 필요에 따라 조정
    SM_Gun->SetRelativeRotation(FRotator::ZeroRotator);
    SM_Gun->SetVisibility(true);

	// 총알이 나가는 위치 씬 컴포넌트 생성 및 SM_Gun에 부착
    FireLocation = CreateDefaultSubobject<USceneComponent>(TEXT("FireLocation"));
    FireLocation->SetupAttachment(SM_Gun);
    FireLocation->SetRelativeLocation(FVector(60.f, 0.f, 15.f)); // 위치는 필요에 따라 조정

    // 수류탄 나가는 위치는 카메라 아래 80 0 0
    GrenadeLocation = CreateDefaultSubobject<USceneComponent>(TEXT("GrenadeLocation"));
    GrenadeLocation->SetupAttachment(FirstPersonCameraComponent);
    GrenadeLocation->SetRelativeLocation(FVector(80.f, 0.f, 0.f)); // 위치는 필요에 따라 조정

	// 태그 추가
    Tags.Add(FName("Player"));

    CharacterWidget = nullptr;
}

void AUNPJCharacter::BeginPlay()
{
    Super::BeginPlay();
    GunIdleRot = SM_Gun->GetRelativeRotation(); // 총의 초기 회전값 저장
    // WBP_Character 위젯 생성 및 화면에 추가
    if( DeathWidgetClass )
    {
        DeadWidget = CreateWidget<UDeadWidget>(GetWorld(), DeathWidgetClass); // 일단 미리 만들어 두기
    }
    if( AbilitySelectWidgetClass )
    {
        AbilitySelectWidget = CreateWidget<UAbilitySelectWidget>(GetWorld(), AbilitySelectWidgetClass); // 일단 미리 만들어 두기
    }
    if (CharacterWidgetClass)
    {
        CharacterWidget = CreateWidget<UCharacterWidget>(GetWorld(), CharacterWidgetClass);
        if (CharacterWidget)
        {
            CharacterWidget->AddToViewport();
            // 체력바에 현재 HP 매핑
            if (CharacterWidget->HealthBar)
            {
                CharacterWidget->HealthBar->SetPercent(CurrentHP / MaxHP);
            }
            // 경험치바에 현재 EXP 매핑
            if (CharacterWidget->ExpBar)
            {
                CharacterWidget->ExpBar->SetPercent(CurrentExp / MaxExp);
            }
            // 총알 UI에 현재 총알 매핑
            if (CharacterWidget->BulletState)
            {
                FString BulletText = FString::Printf(TEXT("%d / %d"), CurrentBullet, MaxBullet);
                CharacterWidget->BulletState->SetText(FText::FromString(BulletText));
            }
            // 체력 텍스트 UI에 현재 체력 매핑
            if (CharacterWidget->HealthState)
            {
                FString HealthText = FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, MaxHP);
                CharacterWidget->HealthState->SetText(FText::FromString(HealthText));
            }
            // 경험치 텍스트 UI에 현재 경험치 매핑
            if (CharacterWidget->ExpState)
            {
                FString ExpText = FString::Printf(TEXT("%.0f / %.0f"), CurrentExp, MaxExp);
                CharacterWidget->ExpState->SetText(FText::FromString(ExpText));
            }
            // 타이머 표시할 텍스트 UI에 매핑
            if (CharacterWidget->TimerState)
            {
                FString TimerText = FString::Printf(TEXT("%.0f"), TimerValue); // 문자열 생성
                CharacterWidget->TimerState->SetText(FText::FromString(TimerText)); // FText로 변환 후 설정
            }
            // 라운드 표시할 텍스트 UI에 매핑
            if (CharacterWidget->RoundState)
            {
                FString RoundText = FString::Printf(TEXT("Round: %d"), RoundValue);
                CharacterWidget->RoundState->SetText(FText::FromString(RoundText));
            }
            // 수류탄 스킬 마스크 프로그래스바 초기화
            if (CharacterWidget->GrenadeSkillMask)
            {
                CharacterWidget->GrenadeSkillMask->SetPercent(0.f);
            }
            // 대쉬 스킬 마스크 프로그래스바 초기화
            if (CharacterWidget->DashSkillMask)
            {
                CharacterWidget->DashSkillMask->SetPercent(0.f);
            }
        }
    }
}

void AUNPJCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 공중에 있으면 상태를 Jumping으로 변경
    if (GetCharacterMovement() && GetCharacterMovement()->IsFalling() && CharacterState == ECharacterState::Idle ) 
    {
        if (CharacterState != ECharacterState::Jumping)
        {
            CharacterState = ECharacterState::Jumping;
        }
    }
    // 1초마다 타이머 1초 증가하며 업데이트 함수 호출
    TimerValue += DeltaTime;
    if (CharacterWidget && CharacterWidget->TimerState && CharacterWidget->RoundState )
    {
        CharacterWidget->TimerState->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), TimerValue)));
    }
    // 라운드 업데이트
    int32 ComputedRound = FMath::FloorToInt(TimerValue / RoundChangeTime ) + 1;
    if (ComputedRound > RoundValue)
    {
        ChangeRound(ComputedRound);
    }
    // Idle 상태에서 + 점프 중 총을 천천히 아래로 복귀
    if (CharacterState == ECharacterState::Idle || CharacterState == ECharacterState::Jumping)
    {
        // 총을 천천히 아래로 복귀
        ReturnGunToIdle(DeltaTime);
    }
    // 쿨타임 진행 중이면 스킬 프로그래스바 감소
    if (!bCanThrowGrenade && CharacterWidget && CharacterWidget->GrenadeSkillMask)
    {
        GrenadeCooldownElapsed += DeltaTime;
        float Percent = 1.f - FMath::Clamp(GrenadeCooldownElapsed / GrenadeCooldown, 0.f, 1.f);
        CharacterWidget->GrenadeSkillMask->SetPercent(Percent);
    }
    // 대쉬 쿨타임 진행 중이면 스킬 프로그래스바 감소
    if (!bCanDash && CharacterWidget && CharacterWidget->DashSkillMask)
    {
        DashCooldownElapsed += DeltaTime;
        float Percent = 1.f - FMath::Clamp(DashCooldownElapsed / DashCooldown, 0.f, 1.f);
        CharacterWidget->DashSkillMask->SetPercent(Percent);
    }

}

//////////////////////////////////////////////////////////////////////////// Input

void AUNPJCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
    
	// 매핑 컨텍스트 추가
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AUNPJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUNPJCharacter::Move);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUNPJCharacter::Look);
		// 총 쏘기
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AUNPJCharacter::Fire);
        // 재장전
        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AUNPJCharacter::Reload);
        // 수류탄 투척
        EnhancedInputComponent->BindAction(GrenadeAction, ETriggerEvent::Started, this, &AUNPJCharacter::ThrowGrenade);
        // 대쉬
        EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AUNPJCharacter::Dash);
	}
}

void AUNPJCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    LastMovementInputVector = MovementVector; // 마지막 입력 저장
    LastMovementInputTime = GetWorld()->GetTimeSeconds(); // 마지막 입력 시간 기록

    if (Controller != nullptr)
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void AUNPJCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AUNPJCharacter::Fire()
{
    // 평상시나 점프 중에서도 총 발사 가능
    if (CharacterState != ECharacterState::Idle && CharacterState != ECharacterState::Jumping) return;
    if (CurrentBullet <= 0)
    {
        //UE_LOG(LogTemp, Warning, TEXT("총알이 없습니다. 재장전 합니다"));
        Reload();
        return;
    }
    //경험치 증가 - 잘 작동함
    SetExp(30.f);
    // 체력 감소 - 잘 작동함
    //SetHP(-10.f);
    // 총알 속도 감소 - 잘 작동함
    //SetBulletSpeed(-1000.f);
    // 총알 최대 갯수 - 잘 작동함
    //SetMaxBullet(1);
    //재장전 시간 3초 증가 - 잘 작동함
    //SetReloadDuration(3.f);
    // 수류탄 범위 감소
    //SetGrenadeRadius(-100.f);
 
    CharacterState = ECharacterState::Firing; // 총 쏘는 상태로 변경
    SetBullet(CurrentBullet - 1); // 총알 하나 감소

    // 총을 항상 초기 위치(GunIdleRot)로 복구 후 애니메이션 시작
    SM_Gun->SetRelativeRotation(GunIdleRot);
    // 총쏘기 애니메이션 시작
    FireElapsed = 0.f;
    GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AUNPJCharacter::FireInterpStep, 0.01f, true);

    // 사운드 생성
    if (FireSound) UGameplayStatics::PlaySoundAtLocation(this, FireSound, FireLocation->GetComponentLocation());

    // 이펙트 생성
    if (FireEffect)
    {
        FTransform EffectTransform = FireLocation->GetComponentTransform();
        EffectTransform.SetScale3D(FVector(0.3f));
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, EffectTransform);
    }

    if (ProjectileClass)
    {
        FVector MuzzleLocation = FireLocation->GetComponentLocation();
        FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
        FVector CameraForward = FirstPersonCameraComponent->GetForwardVector();

        // 1. 카메라에서 일정 거리(예: 10000)까지 라인 트레이스
        FVector TraceEnd = CameraLocation + (CameraForward * 10000.f);

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        FVector TargetPoint = TraceEnd;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd, ECC_Visibility, Params))
        {
            TargetPoint = HitResult.ImpactPoint;

            // 라인 트레이스 히트 지점에 하얀색 원 디버그 그리기
            //DrawDebugSphere(GetWorld(), TargetPoint, 16.f, 16, FColor::White, false, 2.0f);

            // 총구(FireLocation)에서 히트 지점까지 빨간색 선 디버그 그리기
            //DrawDebugLine(GetWorld(), FireLocation->GetComponentLocation(), TargetPoint, FColor::Red, false, 2.0f, 0, 2.0f);
        }
        else
        {
            // 트레이스 미히트 시 끝점에 하얀색 원, 경로 빨간색 선
            //DrawDebugSphere(GetWorld(), TraceEnd, 16.f, 16, FColor::White, false, 2.0f);
            //DrawDebugLine(GetWorld(), CameraLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 2.0f);
        }

        // 2. 총구에서 목표점(TargetPoint) 방향으로 회전값 계산
        FVector ShootDir = (TargetPoint - MuzzleLocation).GetSafeNormal();
        FRotator MuzzleRotation = ShootDir.Rotation();

        // 3. 총알 스폰
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this; // 내 캐릭터를 Owner로 지정
        AUNPJProjectile* Projectile = GetWorld()->SpawnActor<AUNPJProjectile>(
            ProjectileClass,
            MuzzleLocation,
            MuzzleRotation,
            SpawnParams
        );
        Projectile->OwnerCharacter = this;
        //if (Projectile) UE_LOG(LogTemp, Warning, TEXT("Spawned Projectile Owner: %s"), Projectile->GetOwner() ? *Projectile->GetOwner()->GetName() : TEXT("없음"));
    }
}

void AUNPJCharacter::ThrowGrenade()
{
    if (CharacterState != ECharacterState::Idle || !bCanThrowGrenade) return; // 쿨타임 중이면 실행 불가

    bCanThrowGrenade = false; // 수류탄 던질 수 없도록 설정
    //UE_LOG(LogTemp, Warning, TEXT("수류탄 투척!"));

    // 사운드 재생
    if (SkillSound) UGameplayStatics::PlaySoundAtLocation(this, SkillSound, GrenadeLocation->GetComponentLocation());

    // 수류탄 액터 스폰
    if (GrenadeClass)
    {
        FVector MuzzleLocation = GrenadeLocation->GetComponentLocation();
        FRotator MuzzleRotation = GrenadeLocation->GetComponentRotation();

        AUNPJGrenade* Grenade = GetWorld()->SpawnActor<AUNPJGrenade>(
            GrenadeClass,
            MuzzleLocation,
            MuzzleRotation
        );
        if (Grenade)
        {
            Grenade->OwnerCharacter = this; // 수류탄의 소유자를 현재 캐릭터로 설정
        }
    }
    if (CharacterWidget && CharacterWidget->GrenadeSkillMask) // UI에 수류탄 쿨타임 마스크 꽉 채우기
    {
        CharacterWidget->GrenadeSkillMask->SetPercent(1.f); // 쿨타임 시작 시 1로 채움
    }
    // 쿨타임 타이머 시작
    GetWorld()->GetTimerManager().SetTimer(
        GrenadeCooldownTimerHandle,
        this,
        &AUNPJCharacter::ResetGrenadeCooldown,
        GrenadeCooldown,
        false
    );
}

void AUNPJCharacter::Dash()
{
    if ((CharacterState != ECharacterState::Idle && CharacterState != ECharacterState::Jumping) || !bCanDash) return;

    bCanDash = false;

    FVector ForwardDirection = GetActorForwardVector();
    FVector RightDirection = GetActorRightVector();
    FVector2D MovementInput;

    // 마지막 입력 이 오래되거나 없으면 정면으로 대쉬
    if ((GetWorld()->GetTimeSeconds() - LastMovementInputTime > StaleInputThreshold) || LastMovementInputVector.IsNearlyZero())
    {
        MovementInput = FVector2D(0, 1); // 정면
    }
    else
    {
        MovementInput = LastMovementInputVector;
    }

    FVector DashDirection = (ForwardDirection * MovementInput.Y + RightDirection * MovementInput.X).GetSafeNormal();

    // 대쉬 속도는 공중과 지상에서 다르게 설정
    float DashSpeed = (GetCharacterMovement()->IsFalling()) ? AirDashSpeed : GroundDashSpeed;
    FVector DashVelocity = DashDirection * DashSpeed;

    // 공중이면 Y(높이) 고정
    if (GetCharacterMovement()->IsFalling())
    {
        DashVelocity.Z = 0.f;
    }

    // 마찰력 임시로 0
    OriginalGroundFriction = GetCharacterMovement()->GroundFriction;
    GetCharacterMovement()->GroundFriction = 0.f;

    // 실제 고속 이동
    LaunchCharacter(DashVelocity, true, true);

    CharacterState = ECharacterState::Dashing;

    // UI 및 쿨타임 처리
    if (CharacterWidget && CharacterWidget->DashSkillMask)
    {
        CharacterWidget->DashSkillMask->SetPercent(1.f);
    }
    GetWorld()->GetTimerManager().SetTimer(
        DashCooldownTimerHandle,
        this,
        &AUNPJCharacter::ResetDashCooldown,
        DashCooldown,
        false
    );

    // 대쉬 끝나면 마찰력 복원 (예: DashDuration 후)
    GetWorld()->GetTimerManager().SetTimer(
        DashDurationTimerHandle,
        [this]()
        {
            GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
            CharacterState = ECharacterState::Idle;
        },
        DashDuration, false
    );
}

void AUNPJCharacter::SetHP(float AddHP)
{
    CurrentHP = FMath::Clamp(CurrentHP + AddHP, 0.f, MaxHP);

    // 체력바 UI 갱신
    if (CharacterWidget && CharacterWidget->HealthBar)
    {
        CharacterWidget->HealthBar->SetPercent(CurrentHP / MaxHP);
        // 체력 텍스트 갱신
        FString HealthText = FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, MaxHP);
        CharacterWidget->HealthState->SetText(FText::FromString(HealthText));
    }
    // 만약 체력이 0 이하가 되면 캐릭터를 죽음 상태로 설정
    if (CurrentHP <= 0.f)
    {
        // 사운드 재생
        if (DeadSound) UGameplayStatics::PlaySoundAtLocation(this, DeadSound, GetActorLocation());

        // 캐릭터 죽음 처리
        CharacterState = ECharacterState::Dead;
        // DeadWidget 표시
        if (DeadWidget)
        {
            DeadWidget->AddToViewport();
            FString ResultText = FString::Printf(
                TEXT("플레이 타임 : %.0f\n라운드 : %d"),
                TimerValue,
                RoundValue
            );
            DeadWidget->ResultState->SetText(FText::FromString(ResultText));
        }
        UGameplayStatics::SetGamePaused(GetWorld(), true); // 게임 멈춤
        // 마우스 커서 및 UI 입력 활성화
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeUIOnly());
        }
    }
}

void AUNPJCharacter::SetExp(float AddExp)
{
    CurrentExp = FMath::Clamp(CurrentExp + AddExp, 0.f, MaxExp);

    // 경험치바 UI 갱신
    if (CharacterWidget && CharacterWidget->ExpBar)
    {
        CharacterWidget->ExpBar->SetPercent(CurrentExp / MaxExp);
        // 경험치 텍스트 갱신
        FString ExpText = FString::Printf(TEXT("%.0f / %.0f"), CurrentExp, MaxExp);
        CharacterWidget->ExpState->SetText(FText::FromString(ExpText));
    }
    // 경험치가 최대치에 도달하면 레벨업 처리
    if (CurrentExp >= MaxExp)
    {
        // 사운드 재생
        if (LevelUpSound) UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation());

        // 레벨업 처리 (예: 최대 경험치 증가, 현재 경험치 초기화 등)
        CurrentExp = 0.f; // 경험치 초기화
        MaxExp += LevelUpExpIncrease; // 최대 경험치 증가
        CharacterWidget->ExpBar->SetPercent(CurrentExp / MaxExp); 
        FString ExpText = FString::Printf(TEXT("%.0f / %.0f"), CurrentExp, MaxExp);
        CharacterWidget->ExpState->SetText(FText::FromString(ExpText));

        MaxHP += LevelUpHPIncrease; // 최대 체력 증가
        CurrentHP = MaxHP; // 현재 체력도 최대 체력으로 초기화
        SetHP( CurrentHP ); // UI 갱신을 위해 SetHP 호출

        // 레벨업 메시지 출력
        //UE_LOG(LogTemp, Warning, TEXT("레벨업! 새로운 최대 경험치: %.0f"), MaxExp);

        // 어빌리티 셀렉트 위젯 표시
        if (AbilitySelectWidget)
        {
            AbilitySelectWidget->AddToViewport();
            UGameplayStatics::SetGamePaused(GetWorld(), true); // 게임 멈춤

            AbilitySelectWidget->CreateAbilitySelectWidgets();
            // 마우스 커서 및 UI 입력 활성화
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                PC->bShowMouseCursor = true;
                PC->SetInputMode(FInputModeUIOnly());
            }
        }
    }
}

void AUNPJCharacter::SetBullet(int32 NewBullet)
{
    CurrentBullet = FMath::Clamp(NewBullet, 0, MaxBullet);

    // 총알 UI 갱신: "현재 개수 / 맥스 개수" 형식으로 표시
    if (CharacterWidget && CharacterWidget->BulletState)
    {
        FString BulletText = FString::Printf(TEXT("%d / %d"), CurrentBullet, MaxBullet);
        CharacterWidget->BulletState->SetText(FText::FromString(BulletText));
    }
}

void AUNPJCharacter::Reload()
{
    if( CharacterState != ECharacterState::Idle && CharacterState != ECharacterState::Jumping ) return;
    if (CurrentBullet >= MaxBullet) return;
 
    // 사운드 재생
    if (ReloadSound) UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());

    // 총을 항상 초기 위치(GunIdleRot)로 복구 후 애니메이션 시작
    SM_Gun->SetRelativeRotation(GunIdleRot);

    CharacterState = ECharacterState::Reloading; // 재장전 상태로 변경
    ReloadElapsed = 0.f;

    GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AUNPJCharacter::ReloadInterpStep, 0.01f, true);

    //UE_LOG(LogTemp, Warning, TEXT("재장전"));
}

void AUNPJCharacter::RemoveAbilitySelectWidget()
{
    if (AbilitySelectWidget)
    {
        AbilitySelectWidget->RemoveFromParent();
        UGameplayStatics::SetGamePaused(GetWorld(), false);

        // 입력 모드를 게임 전용으로 복원
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            PC->bShowMouseCursor = false;
            PC->SetInputMode(FInputModeGameOnly());
        }
    }
}

void AUNPJCharacter::ReloadInterpStep()
{
    ReloadElapsed += 0.01f;
    float Alpha = FMath::Clamp(ReloadElapsed / ReloadDuration, 0.f, 1.f);

    // 시작값에서 목표값까지 Pitch를 누적 보간
    float NewPitch = FMath::Lerp(GunIdleRot.Pitch, GunIdleRot.Pitch + 360.f, Alpha);
    FRotator NewRot = GunIdleRot;
    NewRot.Pitch = NewPitch;
    SM_Gun->SetRelativeRotation(NewRot);

    if (Alpha >= 1.f)
    {
        GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
        //UE_LOG(LogTemp, Warning, TEXT("재장전 완료"));
        SetBullet(CurrentBullet + MaxBullet);
        // 낙하 중이면 점핑 상태, 아니면 Idle
        if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
        {
            CharacterState = ECharacterState::Jumping;
        }
        else
        {
            CharacterState = ECharacterState::Idle;
        }
    }
}

void AUNPJCharacter::FireInterpStep()
{
    FireElapsed += 0.01f;
    float DeltaPitch = FireDegree * 0.01f / FireDuration; // 한 틱마다 올릴 각도

    FRotator CurrentRot = SM_Gun->GetRelativeRotation();
    CurrentRot.Pitch += DeltaPitch;
    SM_Gun->SetRelativeRotation(CurrentRot);

    if (FireElapsed >= FireDuration)
    {
        GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
        // 낙하 중이면 점핑 상태, 아니면 Idle
        if (GetCharacterMovement() && GetCharacterMovement()->IsFalling())
        {
            CharacterState = ECharacterState::Jumping;
        }
        else
        {
            CharacterState = ECharacterState::Idle;
        }
        // Tick에서 ReturnGunToIdle로 복귀
    }
}

void AUNPJCharacter::ReturnGunToIdle(float DeltaTime)
{
    FRotator CurrentRot = SM_Gun->GetRelativeRotation();
    
    // 천천히 보간 (속도 조절: 10.f)
    FRotator NewRot = FMath::RInterpTo(CurrentRot, GunIdleRot, DeltaTime, 10.f);
    SM_Gun->SetRelativeRotation(NewRot);
}

void AUNPJCharacter::Jump()
{
    Super::Jump();
    //UE_LOG(LogTemp, Warning, TEXT("점프 시작!"));
    // 경험지 감소
    //SetExp(CurrentExp - 10.f);
    // 체력 증가
    //SetHP(CurrentHP + 10.f);
    CharacterState = ECharacterState::Jumping;
}

void AUNPJCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    //UE_LOG(LogTemp, Warning, TEXT("착지!"));
    CharacterState = ECharacterState::Idle;
}


