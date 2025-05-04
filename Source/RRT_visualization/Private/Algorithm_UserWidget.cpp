// Fill out your copyright notice in the Description page of Project Settings.


#include "Algorithm_UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UAlgorithm_UserWidget::NativeConstruct() {
	Super::NativeConstruct();

	reset();

	if (Button_Classic) {
		Button_Classic->OnClicked.AddDynamic(this, &UAlgorithm_UserWidget::click_classic);
	}
	/*if (Button_Bidi) {
		Button_Connect->OnClicked.AddDynamic(this, &UAlgorithm_UserWidget::click_bidirect);
	}*/
	if (Button_Connect) {
		Button_Connect->OnClicked.AddDynamic(this, &UAlgorithm_UserWidget::click_connect);
	}
	if (Button_Other) {
		Button_Other->OnClicked.AddDynamic(this, &UAlgorithm_UserWidget::click_other);
	}
	if (Button_Back) {
		Button_Back->OnClicked.AddDynamic(this, &UAlgorithm_UserWidget::click_bidirect);
	}
}

void UAlgorithm_UserWidget::reset() {
	classic = false;
	bidirectional = false;
	connect = false;
	other = false;
	back = false;
}

void UAlgorithm_UserWidget::click_classic() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked classic in board type menu."));
	classic = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UAlgorithm_UserWidget::click_bidirect() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked bidirectional in board type menu."));
	bidirectional = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UAlgorithm_UserWidget::click_connect() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked connect in board type menu."));
	connect = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UAlgorithm_UserWidget::click_other() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked other RRT in board type menu."));
	other = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UAlgorithm_UserWidget::click_back() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked back in board type menu."));
	back = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}
