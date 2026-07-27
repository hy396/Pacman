// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class ASSETPROJECT_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	void OnPossess(APawn* InPawn) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	void GotoNewDestination();
private:
	class APacmanEnemy* ControlledEnemy;
};
