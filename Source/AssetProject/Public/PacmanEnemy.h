// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PacmanEnemy.generated.h"

/**
 * APacmanEnemy - 吃豆人敌人(幽灵)
 *
 * 项目中的"幽灵",继承自 ACharacter(与 APacman 对称),由 AEnemyController 控制移动。
 *
 * 三态状态机(双 bool 标志,无枚举):
 *
 *      [Normal] ─吃超级豆子→ [Vulnerable] ─被玩家抓住→ [Dead] ─DeadTime 后─→ [Normal]
 *         │                       │                            │
 *     NormalSpeed           VulnerableSpeed                速度 = 0
 *     撞玩家→伤            撞玩家→自己死                  撞玩家→无视(返回)
 *
 * 状态转换:
 *   Normal → Vulnerable: SetVulnerable()(由 AMyGameModeBase::SetEnemiesVulnerable 调用)
 *   Vulnerable → Dead:   OnEnemyBeginOverlay 检测到 APacman 且自身 vulnerable
 *   Dead → Normal:       DeadTimerHandle 到期自动回调 SetNormal()(经过 DeadTime 秒)
 *
 * bRestoreTimer 扩展参数(可选):
 *   SetVulnerable/SetDead 都接受 (bRestoreTimer, Time):
 *     - bRestoreTimer=true → 使用调用方传入的 Time 而不是默认值
 *     - bRestoreTimer=false → 使用 VulnerableTime / DeadTime 默认值
 *   SetDead 的特殊逻辑:bRestoreTimer=true 时即使已 Dead 也重启 Timer(可"延长死亡"),
 *     而 SetVulnerable 在已 Vulnerable 时直接 return(防重入)。
 *
 * 已知约束:
 *   - bIsDead / bIsVulnerable 不是 UPROPERTY,蓝图不可见、不可复制,纯 C++ 内部状态
 *   - SetupPlayerInputComponent 是空实现(敌人不需要玩家输入,移动交给 AAIController)
 *
 * @see AEnemyController        行为控制(随机巡逻 / 追击)
 * @see APacman::InJured        OnEnemyBeginOverlay 在 Normal 状态下调用
 * @see AMyGameModeBase::SetEnemiesVulnerable  群体进入 Vulnerable 状态
 */
UCLASS()
class ASSETPROJECT_API APacmanEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	/** 构造函数:默认值 DeadTime/VulnerableTime=5s,NormalSpeed=300, VulnerableSpeed=100,创建 Enemy StaticMesh */
	APacmanEnemy();

	//敌人外观 StaticMeshComponent(挂在胶囊体下面)
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* Enemy;

	//是否处于死亡状态(非 UPROPERTY,仅 C++ 可见)
	bool bIsDead;

	//是否处于虚弱(可被吃)状态(非 UPROPERTY,仅 C++ 可见)
	bool bIsVulnerable;

	//死亡 → Normal 的 Timer(SetDead 启动)
	FTimerHandle DeadTimerHandle;

	//Vulnerable → Normal 的 Timer(SetVulnerable 启动)
	FTimerHandle VulnerableTimerHandle;

	//死亡状态默认持续时长(秒)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DeadTime;

	//虚弱状态默认持续时长(秒)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float VulnerableTime;

	//正常状态下移动速度(cm/s)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float NormalSpeed;

	//虚弱状态下移动速度(cm/s)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float VulnerableSpeed;
protected:
	/** 初始化:SetMovement(true) + 缓存 StartLocation + 绑定胶囊体 OnComponentBeginOverlap */
	virtual void BeginPlay() override;

public:
	/** 每帧(空) */
	virtual void Tick(float DeltaTime) override;

	/** 绑定玩家输入(空,敌人不需要) */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 进入虚弱状态:启动 VulnerableTimer(用 Time 或 VulnerableTime),bIsVulnerable 守卫防重入 + SetMovement(true) */
	void SetVulnerable(bool bRestoreTimer = false, float Time = 0.0f);

	/** 进入死亡状态:瞬移回 StartLocation + 清 VulnerableTimer + 启动 DeadTimer;
	 *  bRestoreTimer=true 即使已 Dead 也重启 Timer(延长死亡);bIsDead 守卫 */
	void SetDead(bool bRestoreTimer = false, float Time = 0.0f);

	/** 恢复 Normal 状态:清两个 Timer + bIsDead/bIsVulnerable = false + SetMovement(true) */
	void SetNormal();

	/** 启用移动并按当前状态写入 MaxWalkSpeed(Normal→NormalSpeed / Vulnerable→VulnerableSpeed / Dead→0);
	 *  bCanMove=false 时强制 0 */
	void SetMovement(bool bCanMove);

	/** 玩家碰撞回调:仅处理 APacman 重叠;
	 *  Dead→return;Vulnerable→SetDead();Normal→Pacman->InJured() */
	UFUNCTION()
	void OnEnemyBeginOverlay(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult & SweepResult);
private:
	//出生点位置(SetDead 中瞬移回这里)
	FVector StartLocation;
};
