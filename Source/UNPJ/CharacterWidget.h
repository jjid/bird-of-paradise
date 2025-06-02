// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"// 텍스트 블록 헤더
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

    //총알 텍스트 ( 현재 개수 / 최대 개수 )
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BulletState;

    // 체력 텍스트 ( 현재 체력 / 최대 체력 )
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthState;

    // 경험치 텍스트 ( 현재 경험치 / 최대 경험치 )
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ExpState;

    // 시간초 타이머 텍스트
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerState;

    // 라운드 텍스트
    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoundState;

    // 수류탄 스킬 마스크 프로그래스바
    UPROPERTY(meta = (BindWidget))
    UProgressBar* GrenadeSkillMask;

    // 대쉬 스킬 마스크 프로그래스바
    UPROPERTY(meta = (BindWidget))
    UProgressBar* DashSkillMask;
};
