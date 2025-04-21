// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MainMenu_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class RRT_VISUALIZATION_API UMainMenu_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool start;
	bool credits;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void reset();
	UFUNCTION()
	void click_start();
	UFUNCTION()
	void click_credits();
	UFUNCTION()
	void click_exit();

	//Title
	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* Text_Title;

	//Button start
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Start;
	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* Text_Start;

	//Button credits
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Credits;
	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* Text_Credits;

	//Button exit
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Exit;
	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* Text_Exit;
};
