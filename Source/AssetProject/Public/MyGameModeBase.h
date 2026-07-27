// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EGameState : uint8
{
	EMenu,
	EPlay,
	EPause,
	EWin,
	EGameOver,
};

UCLASS()
class ASSETPROJECT_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMyGameModeBase();

	int32 GetPacdotNum() const;
	void SetPacdotNum(int Value);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 PacdotNum;
	
	EGameState GetCurrentState() const;
	void SetCurrentState(EGameState Value);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EGameState CurrentState;
	
protected:
	virtual void BeginPlay() override;

public:
	void StartGame();
	void PauseGame();
	void RestartGame();

	void SetEnemiesVulnerable();//设置所有敌人为易伤

	void RecordEnemiesTimer();
	void RestoreEnemiesTimer();

private:
	TArray<class APacmanEnemy*> Enemies;
	TArray<float> EnemiesTimerRemaining;
};

FORCEINLINE int32 AMyGameModeBase::GetPacdotNum() const
{
	return this->PacdotNum;
}


FORCEINLINE EGameState AMyGameModeBase::GetCurrentState() const
{
	return this->CurrentState;
};

