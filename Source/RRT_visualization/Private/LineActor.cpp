// Fill out your copyright notice in the Description page of Project Settings.

#include "LineActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ALineActor::ALineActor() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    LineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineMesh"));
    RootComponent = LineMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (BoxMesh.Succeeded()) {
        LineMesh->SetStaticMesh(BoxMesh.Object);
    }
}

// Called when the game starts or when spawned
void ALineActor::BeginPlay() {
    Super::BeginPlay();

}

// Called every frame
void ALineActor::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

}

void ALineActor::SetLine(FVector Start, FVector End) {
    FVector Direction = End - Start;
    float Length = Direction.Size();
    FVector MidPoint = (Start + End) / 2;
    FRotator Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();

    SetActorLocation(MidPoint);
    SetActorRotation(Rotation);

    LineMesh->SetWorldScale3D(FVector(Length / 100.0f, 0.05f, 0.05f));
}