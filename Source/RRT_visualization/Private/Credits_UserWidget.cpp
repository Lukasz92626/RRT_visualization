// Fill out your copyright notice in the Description page of Project Settings.


#include "Credits_UserWidget.h"

void UCredits_UserWidget::NativeConstruct() {
	Super::NativeConstruct();

	reset();

	if (Button_Exit) {
		Button_Exit->OnClicked.AddDynamic(this, &UCredits_UserWidget::click_exit);
	}
}

void UCredits_UserWidget::reset() {
	exit = false;
}

void UCredits_UserWidget::click_exit() {
	UE_LOG(LogTemp, Warning, TEXT("Click exit in Credits menu."));
	exit = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}