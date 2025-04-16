// Fill out your copyright notice in the Description page of Project Settings.


#include "NodeActor.h"

// Sets default values
ANodeActor::ANodeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    NodeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Node Collision"));
    RootComponent = NodeCollision;
    //RootComponent = Cast<USceneComponent>(NodeCollision);

    NodeModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Node Model"));
    NodeModel->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
    if (SphereMeshAsset.Succeeded())
    {
        NodeModel->SetStaticMesh(SphereMeshAsset.Object);
    }
}

// Called when the game starts or when spawned
void ANodeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANodeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

