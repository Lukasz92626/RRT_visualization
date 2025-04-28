// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "BoardType_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class RRT_VISUALIZATION_API UBoardType_UserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool prepared; //information about select prepared board on menu
	bool random; //information about select random obstacles board on menu
	bool back; //information about select back to previous menu page
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void reset();
	UFUNCTION()
	void click_prepared();
	UFUNCTION()
	void click_random();
	UFUNCTION()
	void click_back();

	//Button prepared
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Prepared;

	//Button random
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Random;

	//Button back
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Back;
};
