// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "CharacterWidget.generated.h"

UCLASS()
class UNPJ_API UCharacterWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    // 캔버스 패널
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* CanvasPanel;

    // 이미지 1
    UPROPERTY(meta = (BindWidget))
    UImage* ArrowPoint;

    // 체력바
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

	// 경험치 바
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExpBar;
};
