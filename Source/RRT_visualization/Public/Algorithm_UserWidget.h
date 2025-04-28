// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Algorithm_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class RRT_VISUALIZATION_API UAlgorithm_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool classic;
	bool connect;
	bool other;
	bool back;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void reset();
	UFUNCTION()
	void click_classic();
	UFUNCTION()
	void click_connect();
	UFUNCTION()
	void click_other();
	UFUNCTION()
	void click_back();

	//Button classic (orginal) RRT
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Classic;

	//Button RRT connect
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Connect;

	//Button RRT
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Other;
	
	//Button back
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Back;
};
