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
#include "Engine/LocalPlayer.h"
#include "DrawDebugHelpers.h" // 디버그 그리기용 헤더 추가
#include "Blueprint/UserWidget.h" // 위젯 사용을 위한 헤더 추가

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

	// 태그 추가
    Tags.Add(FName("Player"));

    CharacterWidget = nullptr;
}

void AUNPJCharacter::BeginPlay()
{
    Super::BeginPlay();
    GunIdleRot = SM_Gun->GetRelativeRotation(); // 총의 초기 회전값 저장
    // WBP_Character 위젯 생성 및 화면에 추가
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
        }
    }
}

void AUNPJCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Idle 상태에서 + 점프 중 총을 천천히 아래로 복귀
    if (CharacterState == ECharacterState::Idle || CharacterState == ECharacterState::Jumping)
    {
        // 총을 천천히 아래로 복귀
        ReturnGunToIdle(DeltaTime);
    }
}

//////////////////////////////////////////////////////////////////////////// Input

void AUNPJCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
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
	}
}


void AUNPJCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AUNPJCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
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

    if (CharacterState != ECharacterState::Idle) return;
    if (CurrentBullet <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("총알이 없습니다. 재장전 합니다"));
        Reload();
        return;
    }
    CharacterState = ECharacterState::Firing;
    SetBullet(CurrentBullet - 1);

    // 총을 항상 초기 위치(GunIdleRot)로 복구 후 애니메이션 시작
    SM_Gun->SetRelativeRotation(GunIdleRot);
    // 총쏘기 애니메이션 시작
    FireElapsed = 0.f;
    GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AUNPJCharacter::FireInterpStep, 0.01f, true);

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
            DrawDebugSphere(GetWorld(), TargetPoint, 16.f, 16, FColor::White, false, 2.0f);

            // 총구(FireLocation)에서 히트 지점까지 빨간색 선 디버그 그리기
            DrawDebugLine(GetWorld(), FireLocation->GetComponentLocation(), TargetPoint, FColor::Red, false, 2.0f, 0, 2.0f);
        }
        else
        {
            // 트레이스 미히트 시 끝점에 하얀색 원, 경로 빨간색 선
            DrawDebugSphere(GetWorld(), TraceEnd, 16.f, 16, FColor::White, false, 2.0f);
            DrawDebugLine(GetWorld(), CameraLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 2.0f);
        }

        // 2. 총구에서 목표점(TargetPoint) 방향으로 회전값 계산
        FVector ShootDir = (TargetPoint - MuzzleLocation).GetSafeNormal();
        FRotator MuzzleRotation = ShootDir.Rotation();

        // 3. 총알 스폰
        AUNPJProjectile* Projectile = GetWorld()->SpawnActor<AUNPJProjectile>(
            ProjectileClass,
            MuzzleLocation,
            MuzzleRotation
        );
    }
}

void AUNPJCharacter::SetHP(float NewHP)
{
    CurrentHP = FMath::Clamp(NewHP, 0.f, MaxHP);

    // 체력바 UI 갱신
    if (CharacterWidget && CharacterWidget->HealthBar)
    {
        CharacterWidget->HealthBar->SetPercent(CurrentHP / MaxHP);
    }
}

void AUNPJCharacter::SetExp(float NewExp)
{
    CurrentExp = FMath::Clamp(NewExp, 0.f, MaxExp);

    // 경험치바 UI 갱신
    if (CharacterWidget && CharacterWidget->ExpBar)
    {
        CharacterWidget->ExpBar->SetPercent(CurrentExp / MaxExp);
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
    if( CharacterState != ECharacterState::Idle ) return;
    if (CurrentBullet >= MaxBullet) return;

    // 총을 항상 초기 위치(GunIdleRot)로 복구 후 애니메이션 시작
    SM_Gun->SetRelativeRotation(GunIdleRot);

    CharacterState = ECharacterState::Reloading;
    ReloadElapsed = 0.f;

    GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AUNPJCharacter::ReloadInterpStep, 0.01f, true);

    UE_LOG(LogTemp, Warning, TEXT("재장전"));
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
        UE_LOG(LogTemp, Warning, TEXT("재장전 완료"));
        SetBullet(CurrentBullet + MaxBullet);
        CharacterState = ECharacterState::Idle;
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
        CharacterState = ECharacterState::Idle;
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

void AUNPJCharacter::SetReloadDuration(float NewDuration)
{
    ReloadDuration = FMath::Max(0.1f, NewDuration); // 최소 재장전 시간은 0.1초
}

void AUNPJCharacter::Jump()
{
    Super::Jump();
    UE_LOG(LogTemp, Warning, TEXT("점프 시작!"));
    CharacterState = ECharacterState::Jumping;
}

void AUNPJCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    UE_LOG(LogTemp, Warning, TEXT("착지!"));
    CharacterState = ECharacterState::Idle;
}
