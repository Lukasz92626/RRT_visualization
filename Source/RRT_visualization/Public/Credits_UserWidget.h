// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Credits_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class RRT_VISUALIZATION_API UCredits_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool exit; //information about select exit from game on menu

	virtual void NativeConstruct() override;

	UFUNCTION()
	void reset();
	UFUNCTION()
	void click_exit();

	//Button exit
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Exit;
};
