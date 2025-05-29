// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNPJ_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// 타이틀 화면 위젯 블루프린트를 할당할 TSubclassOf 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> TitleScreenWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> OptionsWidgetClass;
	
protected:
	// 생성된 타이틀 화면 위젯 인스턴스
	UPROPERTY()
	UUserWidget* CurrentTitleScreenWidget;

	UPROPERTY()
	UUserWidget* CurrentOptionsWidget;

public:
	// 옵션 위젯을 띄우는 함수 (UMG에서 호출할 수 있도록 BlueprintCallable 선언)
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowOptionsScreen();

	// 옵션 위젯을 숨기는 함수 (UMG에서 호출할 수 있도록 BlueprintCallable 선언)
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideOptionsScreen();	
};
