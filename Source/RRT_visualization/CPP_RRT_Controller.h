// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NodeActor.h"
#include "CuboidActor.h"
#include "LineActor.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"
#include "CPP_RRT_Controller.generated.h"

class RRT;

/**
 * 
 */
UCLASS()
class RRT_VISUALIZATION_API ACPP_RRT_Controller : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this controller's properties
	ACPP_RRT_Controller();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorPath;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorTree;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorStart;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Node Actor")
	TSubclassOf<ANodeActor> NodeActorFinish;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Cuboid Actor")
	TSubclassOf<ACuboidActor> CuboidActorObstacle;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Line Actor")
	TSubclassOf<ALineActor> LineActorTree;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Line Actor")
	TSubclassOf<ALineActor> LineActorPath;

	FRotator standard_rotation; //standard rotation (0.0f, 0.0f, 0.0f)
	FVector standard_position; //standard position (0.0f, 0.0f, 0.0f)

	TArray<ANodeActor*> nodes; //nodes on tree
	TArray<ACuboidActor*> cuboids; //cuboids obstacles
	TArray<ALineActor*> lines; //lines between nodes

	void BeginPlay();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void start_RRT();
	void draw_path(RRT* rrt_class);
	FVector move_cord(FVector move_vector);
};
