// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * AEnemyController - 敌人 AI Controller(随机巡逻)
 *
 * 继承自 AAIController,挂在每个 APacmanEnemy 上(在 BP_PacmanEnemy 中把
 * AIControllerClass 设为本类)实现 NavMesh 上的随机巡逻。
 *
 * 行为循环:
 *   GotoNewDestination() → NavMesh 随机点 + MoveToLocation →
 *   到达后引擎回调 OnMoveCompleted() → 再次 GotoNewDestination()
 *   → 无限循环,敌人永远在 NavMesh 可达范围内乱走
 *
 * 调用入口:
 *   AMyGameModeBase::StartGame() 中对每个敌人调 GotoNewDestination()
 *   启动第一次巡逻(后续自动循环)
 *
 * 已知约束:
 *   - SearchRadius = 10000(单位:UU)非常大,几乎覆盖整个 NavMesh
 *   - AcceptanceRadius = 50:接近 50 单位就算"到了",触发 OnMoveCompleted
 *   - 没有追逐玩家逻辑(Normal 状态下只是随机巡逻,Vulnerable 状态由 APacmanEnemy 自己降速)
 *
 * @see APacmanEnemy          被控制的 Pawn
 * @see AMyGameModeBase::StartGame  首次触发巡逻
 */
UCLASS()
class ASSETPROJECT_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	/** Possess 时缓存 ControlledEnemy = Cast<APacmanEnemy>(InPawn) */
	void OnPossess(APawn* InPawn) override;

	/** 移动完成回调(到达 NavMesh 目标点):再次 GotoNewDestination() 实现无限巡逻 */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	/** 选择新目标:NavMesh 随机可达点 + MoveToLocation(AcceptanceRadius = 50) */
	void GotoNewDestination();
private:
	//缓存被控制的敌人(OnPossess 写入)
	class APacmanEnemy* ControlledEnemy;
};
