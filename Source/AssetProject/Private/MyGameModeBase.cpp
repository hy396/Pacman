// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

#include "EnemyController.h"
#include "EngineUtils.h"
#include "pacdot.h"
#include "Pacman.h"
#include "PacmanEnemy.h"
#include "Engine/Public/EngineUtils.h"

AMyGameModeBase::AMyGameModeBase()
{
	CurrentState = EGameState::EMenu;
}

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	for(TActorIterator<Apacdot> pacdotitr(GetWorld()); pacdotitr; ++pacdotitr)
	{
		PacdotNum++;
	}
	for(TActorIterator<APacmanEnemy> Enemyitr(GetWorld()); Enemyitr; ++Enemyitr)
	{
		Enemies.Add(Cast<APacmanEnemy>(*Enemyitr));
	}
}

void AMyGameModeBase::StartGame()
{
	if(CurrentState == EGameState::EMenu)
	{
		SetCurrentState(EGameState::EPlay);
		for(auto Iter(Enemies.CreateConstIterator()); Iter; ++Iter)
		{
			Cast<AEnemyController>((*Iter)->GetController())->GotoNewDestination();
		}
	}
}

void AMyGameModeBase::PauseGame()
{
	if(GetCurrentState() == EGameState::EPlay)
	{
		RecordEnemiesTimer();
		SetCurrentState(EGameState::EPause);
	}else if(GetCurrentState() == EGameState::EPause)
	{
		RestoreEnemiesTimer();
		SetCurrentState(EGameState::EPlay);
	}
}

void AMyGameModeBase::RestartGame()
{
	GetWorld()->GetFirstLocalPlayerFromController()->ConsoleCommand(TEXT("RestartLevel"));
}

void AMyGameModeBase::SetEnemiesVulnerable()
{
	for(auto Iter(Enemies.CreateConstIterator()); Iter; ++Iter)
	{
		(*Iter)->SetVulnerable();
	}
}

void AMyGameModeBase::RecordEnemiesTimer()
{
	for(auto Iter(Enemies.CreateConstIterator()); Iter; ++Iter)
	{
		if((*Iter)->bIsVulnerable == false && (*Iter)->bIsDead == false)
		{
			EnemiesTimerRemaining.Add(-1);
		}
		if((*Iter)->bIsVulnerable == true && (*Iter)->bIsDead == false)
		{
			float vulnerbleTimerRemaining =	GetWorldTimerManager().GetTimerRemaining((*Iter)->VulnerableTimerHandle);
			EnemiesTimerRemaining.Add(vulnerbleTimerRemaining);
		}
		if((*Iter)->bIsDead == true)
		{
			float vulnerbleTimerRemaining =	GetWorldTimerManager().GetTimerRemaining((*Iter)->VulnerableTimerHandle);
			EnemiesTimerRemaining.Add(vulnerbleTimerRemaining);
		}
	}
}

void AMyGameModeBase::RestoreEnemiesTimer()
{
	for(auto Iter(Enemies.CreateConstIterator()); Iter; ++Iter)
	{
		if((*Iter)->bIsVulnerable == true && (*Iter)->bIsDead == false)
		{
			(*Iter)->SetVulnerable(true,EnemiesTimerRemaining[Iter.GetIndex()]);
		}
		if((*Iter)->bIsDead == true)
		{
			(*Iter)->SetDead(true,EnemiesTimerRemaining[Iter.GetIndex()]);	
		}
	}
}

void AMyGameModeBase::SetPacdotNum(int Value)
{
	PacdotNum = Value;
	if(PacdotNum == 0)
	{
		SetCurrentState(EGameState::EWin);
	}
}

void AMyGameModeBase::SetCurrentState(EGameState Value)
{
	/*
	* 	EMenu,
	EPlay,
	EPause,
	EWin,
	EGameOver,
	 */
	CurrentState = Value;
	switch (CurrentState)
	{
	case EGameState::EMenu:
		break;
	case EGameState::EPlay:
		{
			for(auto Iter(Enemies.CreateIterator()); Iter; ++Iter)
			{
				(*Iter)->SetMovement(true);
			}
			Cast<APacman>(GetWorld()->GetFirstPlayerController()->GetPawn())->SetMovement(true);
		}
		
		break;
	case EGameState::EWin:
		{
			for(auto Iter(Enemies.CreateIterator()); Iter; ++Iter)
			{
				(*Iter)->Destroy();
			}
		}
		break;
	case EGameState::EPause:
	case EGameState::EGameOver:
		{
			for(auto Iter(Enemies.CreateIterator()); Iter; ++Iter)
			{
				(*Iter)->SetMovement(false);
				GetWorldTimerManager().ClearAllTimersForObject(*Iter);
			}
			Cast<APacman>(GetWorld()->GetFirstPlayerController()->GetPawn())->SetMovement(false);
		}
		break;
	default:
		break;
	}
}
