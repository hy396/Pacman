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

/**
 * AMyGameModeBase - 游戏主模式 + 5 态机调度
 *
 * 项目唯一的 GameMode,职责:
 *   1. 维护 EGameState 5 态机(EMenu / EPlay / EPause / EWin / EGameOver)
 *   2. 维护剩余 PacdotNum 计数(归零触发 EWin)
 *   3. 缓存场景中的所有 APacmanEnemy(用于群体操作)
 *   4. 暂停时序列化所有敌人 Timer,恢复时精确继续
 *
 * 状态转换:
 *   EMenu   → StartGame()      → EPlay    (并对所有敌人调 GotoNewDestination 启动 AI)
 *   EPlay   ↔ PauseGame()      ↔ EPause   (记录/恢复所有敌人 vulnerable/dead Timer 剩余)
 *   EPlay   → SetPacdotNum(0)  → EWin     (销毁所有敌人)
 *   EPlay   → SetCurrentState(EGameOver)  (APacman::InJured 在 Lifes==0 时调用)
 *   任意    → SetCurrentState(EPause | EGameOver) → 全员 SetMovement(false) + ClearAllTimersForObject
 *   任意    → SetCurrentState(EPlay)              → 全员 SetMovement(true)
 *
 * SetCurrentState 副作用表(在 cpp switch 中实现):
 *   - EMenu      : 无操作
 *   - EPlay      : 所有敌人 + Pacman 启用移动
 *   - EPause /
 *     EGameOver  : 所有敌人禁用移动 + 清所有敌人相关 Timer + Pacman 禁用移动
 *   - EWin       : Destroy 所有敌人(场景清理)
 *
 * 暂停/恢复的 Timer 序列化:
 *   RecordEnemiesTimer 遍历 Enemies,按状态决定 EnemiesTimerRemaining 中存什么:
 *     - Normal            → -1(哨兵,表示"无 Timer")
 *     - Vulnerable       → GetTimerRemaining(VulnerableTimerHandle)
 *     - Dead              → GetTimerRemaining(VulnerableTimerHandle)(注:此处实际存的是 VulnerableTimerHandle,可能是 bug)
 *   RestoreEnemiesTimer 遍历 Enemies + 调 SetVulnerable(true, t) / SetDead(true, t) 重启 Timer
 *
 * 已知约束:
 *   - BeginPlay 中用 TActorIterator 累加 PacdotNum(从 0 开始),所以 PacdotNum 必须在编辑器中配 0
 *   - EGameOver 触发后敌人不会自动 Destroy(只有 EWin 销毁)
 *
 * @see APacman              受伤/扣血时通知 GameMode
 * @see APacmanEnemy::SetVulnerable  群体虚弱
 * @see AEnemyController     StartGame 时触发 AI 启动
 */
UCLASS()
class ASSETPROJECT_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	/** 构造函数:CurrentState 默认 EMenu */
	AMyGameModeBase();

	/** 内联:获取剩余 Pacdot 数 */
	int32 GetPacdotNum() const;

	/** 设置 Pacdot 数:归零时自动触发 EGameState::EWin */
	void SetPacdotNum(int Value);

	//剩余 Pacdot 数(BeginPlay 中通过 TActorIterator 累加,运行时由 APacman::OnPacmanBeginOverlay 递减)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 PacdotNum;

	/** 内联:获取当前状态 */
	EGameState GetCurrentState() const;

	/** 设置当前状态:同时根据新状态触发对应副作用(见类注释的副作用表) */
	void SetCurrentState(EGameState Value);

	//当前游戏状态(默认 EMenu)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EGameState CurrentState;

protected:
	/** 初始化:遍历场景累加 PacdotNum + 缓存所有 APacmanEnemy 到 Enemies */
	virtual void BeginPlay() override;

public:
	/** 开始游戏(仅在 EMenu 时):切到 EPlay + 触发所有敌人的 AI GotoNewDestination */
	void StartGame();

	/** 切换暂停:在 EPlay/EPause 间切换(EPause 时序列化所有敌人 Timer,EPlay 时恢复) */
	void PauseGame();

	/** 重启关卡:ConsoleCommand "RestartLevel"(完全重置关卡,所有状态清零) */
	void RestartGame();

	/** 群体虚弱:对 Enemies 中每个敌人调 SetVulnerable()(玩家吃超级豆子触发) */
	void SetEnemiesVulnerable();

	/** 暂停时序列化所有敌人的 vulnerable/dead Timer 剩余时间到 EnemiesTimerRemaining */
	void RecordEnemiesTimer();

	/** 恢复时从 EnemiesTimerRemaining 反序列化,按敌人状态重启对应 Timer(SetVulnerable/SetDead 重启) */
	void RestoreEnemiesTimer();

private:
	//缓存的所有敌人(BeginPlay 中 TActorIterator 填充)
	TArray<class APacmanEnemy*> Enemies;

	//暂停时记录的敌人 Timer 剩余时长(-1 表示 Normal 状态无 Timer)
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

