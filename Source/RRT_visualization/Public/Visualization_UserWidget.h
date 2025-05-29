// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include <string>
#include "Visualization_UserWidget.generated.h"

using namespace std;

/**
 * 
 */
UCLASS()
class RRT_VISUALIZATION_API UVisualization_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool exit; //information about select exit from game on menu

	virtual void NativeConstruct() override;

	UFUNCTION()
	void reset();
	UFUNCTION()
	void click_exit();

	void set_algorithm_type(string algorithm_type);
	void set_board_dimensions(double length, double width, double height);
	void set_start_position(double x, double y, double z);
	void set_finish_position(double x, double y, double z);
	void set_total_nodes(int num_nodes);
	void set_total_obstacles(int num_obstacles);
	void set_node_path(int num_nodes);
	void set_path_length(double length);
	void set_time(double time);

	//Algorithm type
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Algorithm;

	//Board dimensions
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_LengthValue;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_WidthValue;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_HeightValue;

	//Start position
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Start_X_cord;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Start_Y_cord;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Start_Z_cord;

	//Finish position
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Finish_X_cord;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Finish_Y_cord;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Finish_Z_cord;

	//Statistics
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_TotalNodesValue;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_TotalObstaclesValue;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_NodesPathValue;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_PathLengthValue;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_TimeValue;

	//Button exit
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Exit;
};
