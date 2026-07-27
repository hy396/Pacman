// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"

#include "NavigationSystem.h"
#include "Pacman.h"
#include "PacmanEnemy.h"

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEnemy = Cast<APacmanEnemy>(InPawn);
}

void AEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	GotoNewDestination();

}

void AEnemyController::GotoNewDestination()
{
	UNavigationSystemV1* NavMesh = UNavigationSystemV1::GetCurrent(this);
	if(NavMesh)
	{
		float SearchRadius = 10000.0f;//搜索半径
		FNavLocation RandomLocation;
		const bool bNewDestinationFound = NavMesh->GetRandomReachablePointInRadius(ControlledEnemy->GetActorLocation(),SearchRadius,RandomLocation);
		if(bNewDestinationFound)
		{
			MoveToLocation(RandomLocation.Location,50.0f);
		}
	}
}
