// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "UNPJCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UStaticMeshComponent; // 추가

UCLASS(config=Game)
class AUNPJCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= C_Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** 1인칭 카메라 하위에 부착되는 스태틱 메시(총) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SM_Gun;

	// 총알이 진짜로 나가는 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Mesh, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FireLocation;

	// 총쏠때 이펙트 나오는 위치 컴포넌트. 하이러키는 총 아래.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = C_Mesh, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FireEffectLocation;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= C_Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= C_Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	// 총알을 쏘는 액션 - 마우스 왼클릭
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	// 총알 쏠 때 나오는 케스케이드 이펙트 . 필요한 헤더 파일 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Effects, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* FireEffect;

	// 총알 액터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C_Actor, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AUNPJProjectile> ProjectileClass;
	
public:
	AUNPJCharacter();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// 총 쏠떄 실행되는 함수
	void Fire();

protected:
	// APawn interface
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

