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
	bool classic;  //information about select classic RRT algorithm
	bool bidirectional; //information about select bidirectional RRT algorithm
	bool connect; //information about select RRT connect algorithm
	bool other;
	bool back; //information about select back to previous menu page

	virtual void NativeConstruct() override;

	UFUNCTION()
	void reset();
	UFUNCTION()
	void click_classic();
	UFUNCTION()
	void click_bidirect();
	UFUNCTION()
	void click_connect();
	UFUNCTION()
	void click_other();
	UFUNCTION()
	void click_back();

	//Button classic (orginal) RRT
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Classic;

	//Button bidirectional RRT
	//UPROPERTY(meta = (BindWidget))
	//class UButton* Button_Bidi;

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
