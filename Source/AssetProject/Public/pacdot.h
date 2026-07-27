// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "pacdot.generated.h"

/**
 * Apacdot - 可收集的豆子(普通 / 超级两种)
 *
 * 项目中最简单的 Actor。场景里散布大量 BP_pacdot(普通豆,小圆点)和少量
 * Mypacdot(超级豆,大圆点),都是本类的蓝图子类,唯一区别是 bIsSuperPacdot。
 *
 * 收集机制(反转设计):
 *   收集逻辑不在 Apacdot 内部,而在 APacman::OnPacmanBeginOverlay 中检测:
 *     - Cast 到 Apacdot 后判断 bIsSuperPacdot:
 *         true  → GameMode->SetEnemiesVulnerable()(所有敌人进入虚弱态)
 *         false → 无额外效果
 *     - 任何情况都 Destroy + GameMode->SetPacdotNum(N-1)
 *
 * 组件结构:
 *   - PacdotCollision(USphereComponent,根):碰撞触发器
 *   - PacdotDisplayMesh(UStaticMeshComponent):视觉外观,挂在根之下
 *
 * 性能:
 *   - PrimaryActorTick.bCanEverTick = false(纯静态物件,不需要 Tick)
 *
 * 蓝图实例:
 *   - BP_pacdot:普通豆,bIsSuperPacdot=false
 *   - Mypacdot:超级豆,bIsSuperPacdot=true
 *
 * @see APacman::OnPacmanBeginOverlay  唯一收集入口
 * @see AMyGameModeBase::SetEnemiesVulnerable  超级豆触发的群体效果
 */
UCLASS()
class ASSETPROJECT_API Apacdot : public AActor
{
	GENERATED_BODY()
	
public:
	/** 构造函数:创建 PacdotCollision(根,USphereComponent) + PacdotDisplayMesh(子级 StaticMesh),
	 *  Tick 已禁用 */
	Apacdot();

	//外观 StaticMeshComponent(挂在碰撞下面)
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* PacdotDisplayMesh;

	//碰撞触发器(根组件,USphereComponent)
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class USphereComponent* PacdotCollision;

	//是否为超级豆(true 时被吃触发 SetEnemiesVulnerable,普通豆仅计数 -1)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsSuperPacdot = false;
protected:
	/** 初始化(空) */
	virtual void BeginPlay() override;

public:
	/** 每帧(空,Tick 已禁用) */
	virtual void Tick(float DeltaTime) override;

};
