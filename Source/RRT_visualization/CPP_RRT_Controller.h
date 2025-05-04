// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NodeActor.h"
#include "CuboidActor.h"
#include "LineActor.h"
#include "MainMenu_UserWidget.h"
#include "Visualization_UserWidget.h"
#include "BoardType_UserWidget.h"
#include "Algorithm_UserWidget.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"
#include <string>
#include "CPP_RRT_Controller.generated.h"

class Point;
class Node;
class Cuboid;
class RRT;
class Bidirectional_RRT;
class RRT_Connect;
class RRT_Star;

/**
 * 
 */
UCLASS()
class RRT_VISUALIZATION_API ACPP_RRT_Controller : public APlayerController
{
	GENERATED_BODY()

	int nodes_path; //number of nodes in path
	RRT* rrt_class; //classic RRT implementation class

	int board_type; //board type selected in menu
	int algorithm; //algorithm selected in menu
public:
	// Sets default values for this controller's properties
	ACPP_RRT_Controller();
	//~ACPP_RRT_Controller();

	UMainMenu_UserWidget* MainMenuInstance; //Instance of main menu class
	UVisualization_UserWidget* VisualizationMenuInstance; //Instance of visualization menu class
	UBoardType_UserWidget* BoardTypeInstance; //Instance of board type menu class
	UAlgorithm_UserWidget* AlgorithmMenuInstance; //Instance of algorithm menu class
	
	//Node actor
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorPath;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorTree;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorStart;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorFinish;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorConnect;

	//Cuboid actor
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Cuboid Actor")
	TSubclassOf<ACuboidActor> CuboidActorObstacle;
	
	//Line actor
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Line Actor")
	TSubclassOf<ALineActor> LineActorTree;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Line Actor")
	TSubclassOf<ALineActor> LineActorPath;

	//User widget
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "User Widget")
	TSubclassOf<UMainMenu_UserWidget> MainMenuUserWidget_Class;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "User Widget")
	TSubclassOf<UVisualization_UserWidget> VisualizationUserWidget_Class;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "User Widget")
	TSubclassOf<UBoardType_UserWidget> BoardTypeUserWidget_Class;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "User Widget")
	TSubclassOf<UAlgorithm_UserWidget> AlgorithmUserWidget_Class;

	FRotator standard_rotation; //standard rotation (0.0f, 0.0f, 0.0f)
	FVector standard_position; //standard position (0.0f, 0.0f, 0.0f)

	TArray<ANodeActor*> nodes; //nodes on tree
	TArray<ACuboidActor*> cuboids; //cuboids obstacles
	TArray<ALineActor*> lines; //lines between nodes

	void BeginPlay();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	double reduce_random_edge(double edge, double cordinate, double board_size);
	void start_RRT();
	void draw_cuboids();
	void set_stats_widget(string algorithm_type);
	//void start_random_map_RRT();
	//void draw_path(RRT* rrt_class);
	//void draw_path(Node* first_node);
	void draw_path();
	void draw_path_connect();
	void clear_map();
	FVector move_cord(FVector move_vector);
	void reset_all_menus();
};
