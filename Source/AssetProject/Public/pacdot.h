// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "pacdot.generated.h"

UCLASS()
class ASSETPROJECT_API Apacdot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Apacdot();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* PacdotDisplayMesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class USphereComponent* PacdotCollision;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsSuperPacdot = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
