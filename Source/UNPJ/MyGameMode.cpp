// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"

AMyGameMode::AMyGameMode()
{
	// 기본적으로 사용될 폰 클래스 설정 (타이틀 화면에서는 플레이어 입력이 필요 없으므로 Null 폰을 사용할 수 있습니다.)
	DefaultPawnClass = nullptr; 
	PlayerControllerClass = APlayerController::StaticClass(); // 기본 PlayerController 사용
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 타이틀 화면 위젯이 설정되어 있다면 생성하여 뷰포트에 추가
	if (TitleScreenWidgetClass)
	{
		CurrentTitleScreenWidget = CreateWidget<UUserWidget>(GetWorld(), TitleScreenWidgetClass);
		if (CurrentTitleScreenWidget)
		{
			CurrentTitleScreenWidget->AddToViewport();

			// UI 모드로 전환하여 마우스 커서 표시 및 UI 입력 활성화
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->bShowMouseCursor = true;
				PlayerController->SetInputMode(FInputModeUIOnly()); // UI 전용 입력 모드
			}
		}
	}
}
