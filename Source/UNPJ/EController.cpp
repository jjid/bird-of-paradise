// EController.cpp

#include "EController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AEController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Ticking... Distance: %f"), Distance);

	// �÷��̾� �� ĳ��
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AEController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!PlayerPawn || !GetPawn()) return;

	// �Ÿ� ���
	float Distance = FVector::Dist(
		PlayerPawn->GetActorLocation(),
		GetPawn()->GetActorLocation()
	);

	if (Distance > StopDistance)
	{
		MoveToActor(PlayerPawn);
	}
	else
	{
		StopMovementImmediately();
	}
}