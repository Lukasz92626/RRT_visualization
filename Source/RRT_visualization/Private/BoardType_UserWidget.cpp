// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardType_UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UBoardType_UserWidget::NativeConstruct() {
	Super::NativeConstruct();

	reset();

	if (Button_Prepared) {
		Button_Prepared->OnClicked.AddDynamic(this, &UBoardType_UserWidget::click_prepared);
	}
	if (Button_Random) {
		Button_Random->OnClicked.AddDynamic(this, &UBoardType_UserWidget::click_random);
	}
	if (Button_Back) {
		Button_Back->OnClicked.AddDynamic(this, &UBoardType_UserWidget::click_back);
	}
}

void UBoardType_UserWidget::reset() {
	prepared = false;
	random = false;
	back = false;
}

void UBoardType_UserWidget::click_prepared() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked prepared in board type menu."));
	prepared = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UBoardType_UserWidget::click_random() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked random in board type menu."));
	random = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UBoardType_UserWidget::click_back() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked back in board type menu."));
	back = true;
}