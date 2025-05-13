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
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

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
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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