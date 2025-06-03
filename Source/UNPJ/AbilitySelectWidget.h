// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityItemWidget.h"
#include "AbilitySelectWidget.generated.h"

UCLASS()
class UNPJ_API UAbilitySelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 능력 선택 할 위젯 전부다 배열에 박아 두기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="C_Ability")
	TArray<TSubclassOf<UAbilityItemWidget>> AbilityItemWidgetClasses;
	
	// 능력 선택 위젯을 넣을 크기 박스 3개
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SelectBox_0;
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SelectBox_1;
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SelectBox_2;

	// 능력 위젯 3개 선택하고 그거 가로 박스에 넣는 함수
	UFUNCTION(BlueprintCallable, Category="C_Ability")
	void CreateAbilitySelectWidgets();
};
