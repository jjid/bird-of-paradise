// EController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EController.generated.h"

UCLASS()
class UNPJ_API AEController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	// ���� �Ÿ� �ȿ� ������ ���߰� �� �Ÿ� (�����Ϳ��� ���� ����)
	UPROPERTY(EditAnywhere, Category = "AI")
	float StopDistance = 300.f;

	// ���� �÷��̾� ĳ��
	UPROPERTY()
	APawn* PlayerPawn = nullptr;
};