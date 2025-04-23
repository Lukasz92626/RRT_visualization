// Fill out your copyright notice in the Description page of Project Settings.


#include "Visualization_UserWidget.h"

void UVisualization_UserWidget::NativeConstruct() {
	Super::NativeConstruct();

	reset();

	if (Button_Exit) {
		Button_Exit->OnClicked.AddDynamic(this, &UVisualization_UserWidget::click_exit);
	}
}

void UVisualization_UserWidget::reset() {
	exit = false;
}

void UVisualization_UserWidget::click_exit() {
	UE_LOG(LogTemp, Warning, TEXT("Click exit in Main Menu."));
	exit = true;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UVisualization_UserWidget::set_board_dimensions(double length, double width, double height) {
	TextBlock_LengthValue->SetText(FText::FromString(FString::SanitizeFloat(length)));
	TextBlock_WidthValue->SetText(FText::FromString(FString::SanitizeFloat(width)));
	TextBlock_HeightValue->SetText(FText::FromString(FString::SanitizeFloat(height)));
}

void UVisualization_UserWidget::set_total_nodes(int num_nodes) {
	TextBlock_TotalNodesValue->SetText(FText::FromString(FString::FromInt(num_nodes)));
}

void UVisualization_UserWidget::set_total_obstacles(int num_obstacles) {
	TextBlock_TotalObstaclesValue->SetText(FText::FromString(FString::FromInt(num_obstacles)));
}

void UVisualization_UserWidget::set_node_path(int num_nodes) {
	TextBlock_NodesPathValue->SetText(FText::FromString(FString::FromInt(num_nodes)));
}

void UVisualization_UserWidget::set_path_length(double length) {
	TextBlock_PathLengthValue->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), length)));
}