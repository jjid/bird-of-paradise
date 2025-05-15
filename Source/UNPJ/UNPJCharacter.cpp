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

	// 총알 이펙트 위치 씬 컴포넌트 생성 및 SM_Gun에 부착
	FireEffectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("FireEffectLocation"));
	FireEffectLocation->SetupAttachment(SM_Gun);
	FireEffectLocation->SetRelativeLocation(FVector(60.f, 0.f, 15.f)); // 이펙트 위치는 필요에 따라 조정

	// 총알이 진짜로 나가는 위치 씬 컴포넌트 생성 및 카메라에 부착
	FireLocation = CreateDefaultSubobject<USceneComponent>(TEXT("FireLocation"));
	FireLocation->SetupAttachment(FirstPersonCameraComponent);
	FireLocation->SetRelativeLocation(FVector(100.f, 0.f, 0.f));

	// 태그 추가
    Tags.Add(FName("Player"));
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
	// 일단 총을 소묭ULOG로 발사 라고 찍기
	UE_LOG(LogTemp, Warning, TEXT("Fire!"));

	// 총알을 쏘면 FireLocation 위치에 FireEffect 이펙트를 생성 (크기는 0.3배로 설정)
	if (FireEffect)
	{
		FTransform EffectTransform = FireEffectLocation->GetComponentTransform();
		EffectTransform.SetScale3D(FVector(0.2f)); // 이펙트 크기를 0.3배로 설정
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, EffectTransform);
	}

	// 총알 발사. 방시은 스폰 액터. 위치치는 이펙트와 동일.
	if (ProjectileClass)
	{
		FVector SpawnLocation = FireLocation->GetComponentLocation();
		FRotator SpawnRotation = FirstPersonCameraComponent->GetComponentRotation();

		// 총알 스폰, 회전은 카메라가 보는 방향으로
		AUNPJProjectile* Projectile = GetWorld()->SpawnActor<AUNPJProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	}

}