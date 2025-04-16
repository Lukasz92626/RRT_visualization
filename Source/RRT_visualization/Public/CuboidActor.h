// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CuboidActor.generated.h"

UCLASS()
class RRT_VISUALIZATION_API ACuboidActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACuboidActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Metoda do ustawiania wymiarów
	UFUNCTION(BlueprintCallable, Category = "Box Control")
	void SetBoxDimensions(const FVector& NewDimensions);

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BoxMesh;

private:
	// Wewnêtrzne przechowywanie wymiarów
	FVector BoxDimensions;

	// Zastosowanie skalowania
	void UpdateBoxScale();

};
