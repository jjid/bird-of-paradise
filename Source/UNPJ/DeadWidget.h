// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h" // 텍스트 블록 헤더
#include "DeadWidget.generated.h"

UCLASS()
class UNPJ_API UDeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 결과 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultState;
	
};
