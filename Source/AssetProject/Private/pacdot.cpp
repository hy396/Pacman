// Fill out your copyright notice in the Description page of Project Settings.


#include "pacdot.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
Apacdot::Apacdot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	
	PacdotDisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PacdotDisplayMesh"));
	PacdotCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PacdotCollision"));

	PacdotDisplayMesh->SetupAttachment(PacdotCollision);

}

// Called when the game starts or when spawned
void Apacdot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Apacdot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

