// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu_UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenu_UserWidget::NativeConstruct() {
	Super::NativeConstruct();

	reset();

	if (Button_Start) {
		Button_Start->OnClicked.AddDynamic(this, &UMainMenu_UserWidget::click_start);
	}
	if (Button_Credits) {
		Button_Credits->OnClicked.AddDynamic(this, &UMainMenu_UserWidget::click_credits);
	}
	if (Button_Exit) {
		Button_Exit->OnClicked.AddDynamic(this, &UMainMenu_UserWidget::click_exit);
	}
}

void UMainMenu_UserWidget::reset() {
	start = false;
	credits = false;
}

void UMainMenu_UserWidget::click_start() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked Start in Main Menu."));
	start = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenu_UserWidget::click_credits() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked Credits in Main Menu."));
	credits = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenu_UserWidget::click_exit() {
	UE_LOG(LogTemp, Warning, TEXT("Clicked Exit in Main Menu."));
	if (GetWorld()->IsGameWorld()) {
		FGenericPlatformMisc::RequestExit(false);
	}
	else {
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
	}
}