// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pacman.generated.h"

/**
 * APacman - 吃豆人玩家
 *
 * 项目主角,继承自 ACharacter(用 CharacterMovementComponent 处理 4 方向移动)。
 *
 * 核心机制:
 *   - 4 方向移动:MoveX / MoveY 轴输入,正负值决定朝向(FRotator(0, 0/±90/180, 0))
 *   - 生命系统:Lifes(默认 3),归零 → GameMode->SetCurrentState(EGameOver)
 *   - 无敌时间:受伤后切到自定义 "wudi" 碰撞 Profile,wuditime(默认 3s)后 Resetting 切回 "Pawn"
 *   - 道具收集:OnPacmanBeginOverlay 检测 Apacdot,如果是 SuperPacdot → GameMode->SetEnemiesVulnerable()
 *
 * 输入约定(UE4 经典,不是 Enhanced Input,引擎版本 4.27):
 *   - Actions: "StartGame" / "PauseGame" / "RestartGame"(都是 IE_Pressed)
 *   - Axes:    "MoveX" / "MoveY"(直接传 float 值)
 *   - 所有移动相关回调都先查 GameModeRef->GetCurrentState() == EGameState::EPlay 才执行
 *
 * 子系统依赖:
 *   - AMyGameModeBase  状态机查询(SetCurrentState / GetCurrentState) + 游戏流程
 *   - Apacdot          道具收集(OnPacmanBeginOverlay)
 *   - APacmanEnemy     受伤由外部调用 InJured 触发(目前是谁调用未明,见 cpp 注释)
 *
 * 已知约束:
 *   - "wudi" CollisionProfile 必须在 Config/DefaultEngine.ini 中预先定义,否则 SetCollisionProfileName 会 warn
 *   - 移动用 Velocity.X / Velocity.Y 累加器,但 AddMovementInput 只接收一次方向后未清零
 *     (依赖轴绑定每帧推一个值,因此无副作用)
 *
 * @see AMyGameModeBase      游戏状态 + 计分
 * @see Apacdot              收集物
 * @see APacmanEnemy         被追对象
 */
UCLASS()
class ASSETPROJECT_API APacman : public ACharacter
{
	GENERATED_BODY()

public:
	/** 构造函数:默认 Lifes=3 / NormalSpeed=600 / wuditime=3s,创建并附加 Pacman StaticMeshComponent */
	APacman();

	//玩家外观 StaticMeshComponent(挂在胶囊体下面,真正的"豆子"视觉)
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* Pacman;

	//剩余生命数(归零触发 EGameState::EGameOver)
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 Lifes;

	//受伤后无敌时长(秒),InJured → wuditime 秒后 Resetting 切回正常碰撞
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float wuditime;

	//正常移动速度(cm/s),由 SetMovement(true) 写入 GetCharacterMovement()->MaxWalkSpeed
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float NormalSpeed;
protected:
	/** 初始化:缓存 StartLocation 与 GameModeRef + SetMovement(true) + 绑定胶囊体 OnComponentBeginOverlap */
	virtual void BeginPlay() override;

public:
	/** 每帧调用(目前为空) */
	virtual void Tick(float DeltaTime) override;

	/** 绑定 UE4 经典输入:StartGame/PauseGame/RestartGame Actions + MoveX/MoveY Axes */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 受伤处理:切 "wudi" 碰撞 + Lifes-- + 归零则 EGameOver,否则瞬移回 StartLocation + 启动 wuditime 计时 */
	void InJured();

	/** 无敌时间结束回调:ClearTimer + 切回 "Pawn" 碰撞(可再次受伤) */
	void Resetting();

	/** 启用/禁用移动:true 写 NormalSpeed 到 MaxWalkSpeed,false 写 0 */
	void SetMovement(bool bCanMove);

	/** 转发到 GameModeRef->StartGame() */
	void StartGame();

	/** 转发到 GameModeRef->PauseGame() */
	void PauseGame();

	/** 转发到 GameModeRef->RestartGame() */
	void RestartGame();

	/** 水平轴输入(仅在 EPlay 状态生效):设置朝向(±X)+ Velocity.X = Value + AddMovementInput */
	void MoveX(float Value);

	/** 垂直轴输入(仅在 EPlay 状态生效):设置朝向(±Y)+ Velocity.Y = Value + AddMovementInput */
	void MoveY(float Value);

	/** 道具碰撞回调:EPlay 状态下捡 Apacdot,SuperPacdot → SetEnemiesVulnerable,否则仅 Destroy + 计数 -1 */
	UFUNCTION()
	void OnPacmanBeginOverlay(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult & SweepResult);
private:

	//出生点位置(InJured 中瞬移回这里重置)
	FVector StartLocation;

	//当前移动方向(MoveX / MoveY 写入,AddMovementInput 使用)
	FVector Velocity;

	//缓存的 GameMode(BeginPlay 中查询,所有状态判断 / 流程调用都走它)
	class AMyGameModeBase* GameModeRef;

	//无敌时间 Timer(InJured 启动,Resetting 到期回调清理)
	FTimerHandle ResettingTimerHandle;
};
