// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySelectWidget.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UNPJ/AbilitySelectWidget.h"
#include "UNPJ/UNPJCharacter.h"


void UAbilitySelectWidget::CreateAbilitySelectWidgets()
{
    // 이전에 있던 거 다 삭제
    if (SelectBox_0) SelectBox_0->ClearChildren();
    if (SelectBox_1) SelectBox_1->ClearChildren();
    if (SelectBox_2) SelectBox_2->ClearChildren();
    // 인덱스 배열 생성 및 셔플
    TArray<int32> Indices;
    for (int32 i = 0; i < AbilityItemWidgetClasses.Num(); ++i)
        Indices.Add(i);
    Indices.Sort([](int32, int32) { return FMath::RandBool(); }); // 간단한 셔플

    // 3개만 사용
    for (int32 i = 0; i < 3; ++i)
    {
        int32 RandomIdx = Indices[i];
        if (AbilityItemWidgetClasses.IsValidIndex(RandomIdx))
        {
            UUserWidget* AbilityWidget = CreateWidget<UUserWidget>(GetWorld(), AbilityItemWidgetClasses[RandomIdx]);
            if (AbilityWidget)
            {
                switch (i)
                {
                case 0:
                    SelectBox_0->AddChild(AbilityWidget); // SizeBox에 맞게 AddChild 사용
                    break;
                case 1:
                    SelectBox_1->AddChild(AbilityWidget);
                    break;
                case 2:
                    SelectBox_2->AddChild(AbilityWidget);
                    break;
                }
            }
        }
    }
}