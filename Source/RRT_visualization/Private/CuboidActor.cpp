// Fill out your copyright notice in the Description page of Project Settings.

#include "CuboidActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACuboidActor::ACuboidActor() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Komponent Mesh
    BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
    RootComponent = BoxMesh;

    // Domyœlny mesh — cube
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        BoxMesh->SetStaticMesh(CubeMesh.Object);
    }

    // Domyœlne wymiary
    BoxDimensions = FVector(100.0f, 100.0f, 100.0f);
    UpdateBoxScale();
}

// Called when the game starts or when spawned
void ACuboidActor::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void ACuboidActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}


void ACuboidActor::SetBoxDimensions(const FVector& NewDimensions) {
    BoxDimensions = NewDimensions;
    UpdateBoxScale();
}

void ACuboidActor::UpdateBoxScale() {
    // Poniewa¿ bazowy cube ma rozmiar 100x100x100, dzielimy przez 100
    FVector NewScale = BoxDimensions / 100.0f;
    BoxMesh->SetWorldScale3D(NewScale);
}