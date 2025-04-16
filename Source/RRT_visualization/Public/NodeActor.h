// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"

UCLASS()
class RRT_VISUALIZATION_API ANodeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANodeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ATTRIBUTES")
	class UStaticMeshComponent* NodeModel;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ATTRIBUTES")
	class USphereComponent* NodeCollision;
};
