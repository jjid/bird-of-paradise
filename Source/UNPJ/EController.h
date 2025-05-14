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
	// 일정 거리 안에 들어오면 멈추게 할 거리 (에디터에서 조정 가능)
	UPROPERTY(EditAnywhere, Category = "AI")
	float StopDistance = 300.f;

	// 따라갈 플레이어 캐싱
	UPROPERTY()
	APawn* PlayerPawn = nullptr;
};