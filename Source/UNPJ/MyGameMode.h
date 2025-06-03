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

protected:
	// 생성된 타이틀 화면 위젯 인스턴스
	UPROPERTY()
	UUserWidget* CurrentTitleScreenWidget;

};
