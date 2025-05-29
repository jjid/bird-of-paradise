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

void AMyGameMode::ShowOptionsScreen()
{
	// 옵션 위젯이 이미 띄워져 있다면 무시
	if (CurrentOptionsWidget && CurrentOptionsWidget->IsInViewport())
	{
		return;
	}

	// 기존 타이틀 화면 위젯이 있다면 숨김 (선택 사항)
	if (CurrentTitleScreenWidget)
	{
		CurrentTitleScreenWidget->SetVisibility(ESlateVisibility::Collapsed); // 또는 RemoveFromParent();
	}

	if (OptionsWidgetClass)
	{
		CurrentOptionsWidget = CreateWidget<UUserWidget>(GetWorld(), OptionsWidgetClass);
		if (CurrentOptionsWidget)
		{
			CurrentOptionsWidget->AddToViewport();
            
			// UI 입력 모드 유지 (마우스 커서 표시)
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->SetInputMode(FInputModeUIOnly()); 
				PlayerController->bShowMouseCursor = true;
			}
		}
	}
}

void AMyGameMode::HideOptionsScreen()
{
	if (CurrentOptionsWidget)
	{
		CurrentOptionsWidget->RemoveFromParent(); // 뷰포트에서 제거
		CurrentOptionsWidget = nullptr; // nullptr로 설정하여 메모리 관리

		// 타이틀 화면 위젯이 있다면 다시 보이게 함
		if (CurrentTitleScreenWidget)
		{
			CurrentTitleScreenWidget->SetVisibility(ESlateVisibility::Visible);
		}
        
		// 다시 UI Only 모드로 전환 (확실하게)
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			PlayerController->SetInputMode(FInputModeUIOnly()); 
			PlayerController->bShowMouseCursor = true;
		}
	}
}