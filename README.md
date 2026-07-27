# 🟡 Pacman（吃豆人 Demo）

> 基于 **Unreal Engine 4.27** 制作的 3D 吃豆人风格小游戏。玩家操控吃豆人在迷宫里收集豆子,躲避随机巡逻的幽灵;吃到能量豆后可反击,将幽灵送回巢穴。

![UE](https://img.shields.io/badge/Unreal_Engine-4.27-0D47A1?logo=unrealengine&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows)
![Language](https://img.shields.io/badge/C++-14-00599C?logo=c%2B%2B&logoColor=white)
![AI](https://img.shields.io/badge/AI-NavMesh_Random-9C27B0)
![Status](https://img.shields.io/badge/Status-学习项目-orange)

---

## 📑 目录

- [🎮 游戏概览](#-游戏概览)
- [✨ 游戏特色](#-游戏特色)
- [🛠 技术栈](#-技术栈)
- [💻 系统要求](#-系统要求)
- [🚀 快速开始](#-快速开始)
- [🎯 玩法与操作](#-玩法与操作)
- [📁 项目结构](#-项目结构)
- [🏗 核心架构](#-核心架构)
- [🧩 关键设计模式](#-关键设计模式)
- [🛠 开发指南](#-开发指南)
- [🐞 已知问题与 TODO](#-已知问题与-todo)
- [📜 许可证](#-许可证)
- [🙏 致谢](#-致谢)

---

## 🎮 游戏概览

**Pacman** 是一个完整的 3D 吃豆人 Demo,用 UE4 C++ 实现核心玩法:玩家、敌人(幽灵)、豆子、能量豆、游戏状态机全部由 C++ 驱动,蓝图只承担 UI 和场景配置。

- 🟡 **玩家 3 条命**,归零即 GameOver
- 👻 **4 只幽灵**随机巡逻(NavMesh 随机可达点)
- ⚡ **能量豆机制**:吃到后所有幽灵变虚弱 5 秒,可反杀
- ⏸ **完整暂停系统**:暂停时保留所有幽灵的状态计时,恢复时无缝继续
- 🏆 **通关条件**:吃掉地图上全部豆子(由 GameMode 自动判定)

---

## ✨ 游戏特色

### 🎮 输入与移动
- 采用 **UE4 经典输入系统**(`InputAction` / `InputAxis`),通过 `BindAxis("MoveX", MoveY)` 实现四方向格子化移动
- 玩家朝向自动根据输入方向旋转(`FRotator(0, Yaw, 0)`)
- 速度控制通过 `MaxWalkSpeed` 动态切换

### 👻 AI 敌人
- 每个敌人由独立的 `AEnemyController : AAIController` 控制
- AI 通过 `UNavigationSystemV1::GetRandomReachablePointInRadius` 在 10000 单位半径内选点
- `OnMoveCompleted` 回调中再次寻路,实现**持续随机巡逻**

### 💎 敌人三态机制
- **Normal**(速度 300) → **Vulnerable**(速度 100,蓝色,可被吃掉) → **Dead**(回出生点,5 秒 CD 后复活)
- 状态切换全部用 `FTimerHandle` 计时

### ⏸ 暂停状态保存
- 暂停时遍历所有敌人,记录剩余计时
- 恢复时按记录的剩余值重建 `SetTimer`,实现"暂停期间不计时"

### 🎯 受伤无敌
- 玩家受伤后 `CapsuleComponent` 切换到自定义 Profile `"wudi"`,3 秒内不会再次受伤
- 受伤后回到出生点

---

## 🛠 技术栈

| 类别 | 技术 |
| --- | --- |
| 引擎 | Unreal Engine 4.27 |
| 角色 | `ACharacter` + `UStaticMeshComponent` |
| 碰撞 | `UCapsuleComponent` + `USphereComponent`(豆子触发器) |
| AI | `AAIController` + `UNavigationSystemV1`(NavMesh) |
| 输入 | UE4 经典 `InputComponent`(`BindAction` / `BindAxis`) |
| UI | UMG (`UUserWidget` + `BindWidget`) |
| 模块依赖 | `Core`、`CoreUObject`、`Engine`、`InputCore`、`NavigationSystem`、`AIModule` |

---

## 💻 系统要求

### 运行环境
- **操作系统**:Windows 10 / 11 (64-bit)
- **GPU**:支持 DirectX 11 / 12
- **内存**:≥ 8 GB RAM
- **硬盘**:≥ 5 GB 可用空间

### 开发环境
- **Unreal Engine 4.27**(通过 Epic Games Launcher 安装)
- **Visual Studio 2019 / 2022**(需安装 `Game Development with C++` 工作负载)
- **Git**(可选,用于克隆仓库)

---

## 🚀 快速开始

### 方式一:在 UE 编辑器中打开(推荐)

```bash
# 1. 克隆仓库
git clone https://github.com/<your-username>/AssetProject.git
cd AssetProject

# 2. 双击 AssetProject.uproject,UE 编辑器会自动启动并加载项目
#    (首次打开会触发 C++ 模块编译)

# 3. 编辑器加载完成后,按工具栏 ▶ Play 即可运行游戏
```

### 方式二:命令行构建

```bat
REM 1. 生成 Visual Studio 项目文件
"<UE_INSTALL_DIR>/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" ^
    -projectfiles ^
    -project="%CD%/AssetProject.uproject" ^
    -game -engine -progress

REM 2. 编译编辑器模块
"<UE_INSTALL_DIR>/Engine/Build/BatchFiles/Build.bat" ^
    AssetProjectEditor Win64 Development ^
    -project="%CD%/AssetProject.uproject" -waitmutex

REM 3. 启动编辑器
"<UE_INSTALL_DIR>/Engine/Binaries/Win64/UE4Editor.exe" ^
    "%CD%/AssetProject.uproject"
```

> 💡 将 `<UE_INSTALL_DIR>` 替换为你的 UE 4.27 安装路径,例如 `C:/Program Files/Epic Games/UE_4.27`。

### ⚠️ 首次打开常见问题

| 现象 | 解决方案 |
| --- | --- |
| `LogNavigationSystem: Warning` 找不到 NavMesh | 打开地图后 `RecastNavMesh-Default` → Build,或按 `P` 预览 NavMesh 是否覆盖可走区域 |
| 玩家穿过墙体 | 确认地图中墙体是 `BlockAll` 静态网格,且已构建 NavMesh |
| 输入无响应 | `Edit → Project Settings → Input` 确认 `MoveX` / `MoveY` / `StartGame` / `PauseGame` 已配置 |
| C++ 编译失败 | 通过 Visual Studio Installer 安装 `.NET Framework 4.6.2 Targeting Pack` 和 `Windows 10 SDK` |

---

## 🎯 玩法与操作

### 键位

| 操作 | 键位 | 说明 |
| --- | --- | --- |
| 🏃 水平移动 | `A` / `D` 或 `←` / `→` | X 轴移动,自动旋转 0° / 180° |
| 🏃 垂直移动 | `W` / `S` 或 `↑` / `↓` | Y 轴移动,自动旋转 ±90° |
| ▶ 开始游戏 | 任意 `StartGame` 绑定键 | 触发 `AMyGameModeBase::StartGame()`,进入 EPlay 状态 |
| ⏸ 暂停/继续 | 任意 `PauseGame` 绑定键 | EPause ↔ EPlay 切换,保留所有敌人计时 |
| 🔄 重新开始 | 任意 `RestartGame` 绑定键 | `ConsoleCommand("RestartLevel")` |

### 收集物

| 图标 | 名称 | 效果 |
| --- | --- | --- |
| 🟡 | 普通豆子 | 计数 -1,全部吃完 → EWin |
| ⚡ | 能量豆(`bIsSuperPacdot=true`) | 计数 -1 + 所有敌人进入 Vulnerable 5 秒 |

### 敌人状态

| 状态 | 速度 | 颜色 | 被玩家碰到 |
| --- | --- | --- | --- |
| 🟦 Normal | 300 | 红/粉/蓝/绿(原色) | 玩家 `InJured()` |
| 🔵 Vulnerable | 100 | 蓝色 | 敌人 `SetDead()`,回出生点 5 秒 |
| ⚫ Dead | 0 | 不可见/淡色 | 无碰撞(`SetCollisionProfileName("wudi")`) |

### 受伤与死亡

- 玩家被敌人碰到 → `Lifes--` → 切到 `wudi` Profile 3 秒无敌 → `SetActorLocation(StartLocation)`
- `Lifes == 0` → 切换 `EGameState::EGameOver`,所有敌人停止移动并清空所有 Timer

### 通关

- `PacdotNum == 0` → 自动进入 `EGameState::EWin`,销毁所有敌人
- 由 `WBP_ManUI` 显示胜利界面

---

## 📁 项目结构

```
AssetProject/
├── AssetProject.uproject          # UE 项目描述(4.27,带 AIModule 依赖)
├── AssetProject.sln               # Visual Studio 解决方案
├── Config/                        # 引擎配置
│   ├── DefaultEngine.ini          # 默认关卡、GameMode、碰撞 Profile、自定义 "wudi"
│   ├── DefaultGame.ini
│   ├── DefaultInput.ini           # 经典输入轴配置(MoveX/MoveY/StartGame 等)
│   └── DefaultEditor.ini
├── Source/
│   ├── AssetProject.Target.cs     # Game target
│   ├── AssetProjectEditor.Target.cs
│   └── AssetProject/              # C++ 源码(单模块)
│       ├── AssetProject.Build.cs  # 模块依赖(Core + Engine + NavigationSystem)
│       ├── AssetProject.{h,cpp}   # 模块入口
│       ├── Public/
│       │   ├── Pacman.{h}         # 玩家 ACharacter
│       │   ├── PacmanEnemy.{h}    # 敌人(幽灵)ACharacter
│       │   ├── pacdot.{h}         # 豆子 AActor
│       │   ├── MyGameModeBase.{h} # 游戏状态机
│       │   └── EnemyController.{h}# 敌人 AI 控制器
│       └── Private/
│           └── *.cpp              # 对应实现
├── Content/
│   ├── Audios/                    # BGM.uasset + BGM_Cue.uasset
│   ├── BP/                        # 蓝图资源
│   │   ├── BP_GameModeBase.uasset # GameMode 实例(绑 MyGameModeBase)
│   │   ├── BP_Pacman.uasset       # 玩家实例
│   │   ├── BP_PacmanEnemy.uasset  # 敌人实例
│   │   ├── BP_pacdot.uasset       # 普通豆子
│   │   ├── Mypacdot.uasset        # 能量豆?
│   │   └── WBP_ManUI.uasset       # UI Widget
│   ├── M/                         # 幽灵材质变体(M_red/blue/green/pink/orig 等)
│   ├── map/                       # Map.umap + Map_BuiltData
│   ├── Collections/
│   └── Developers/
└── Script/                        # 占位(空)
```

---

## 🏗 核心架构

### 类关系图

```
ACharacter
 ├── APacman            # 玩家 3 条命 + 无敌 + 重生
 └── APacmanEnemy       # 敌人:Normal / Vulnerable / Dead 三态

AActor
 └── Apacdot            # 豆子(bIsSuperPacdot 标记能量豆)

AGameModeBase
 └── AMyGameModeBase    # EGameState 状态机 + 暂停/恢复 + 通关判定

AAIController
 └── AEnemyController   # NavMesh 随机巡逻
```

### 数据流 — 主循环

```
玩家输入(BindAxis MoveX/MoveY)
        ↓
APacman::MoveX / MoveY  (状态门控:仅 EPlay)
        ↓
AddMovementInput + SetActorRotation(按方向)
        ↓
玩家 ↔ 豆子:OnPacmanBeginOverlay
        ↓
  ├─ 普通豆   → SetPacdotNum(N-1) + Destroy
  └─ 能量豆   → SetEnemiesVulnerable() + SetPacdotNum(N-1) + Destroy
        ↓
PacdotNum == 0 → EGameState::EWin → 销毁所有敌人
```

### 数据流 — 敌人 AI 与战斗

```
BeginPlay
  ↓
AEnemyController::OnPossess → 保存 ControlledEnemy
  ↓
GameMode::StartGame → GotoNewDestination()
  ↓
NavMesh.GetRandomReachablePointInRadius(10000) → MoveToLocation
  ↓
OnMoveCompleted → GotoNewDestination()  (循环)
  ↓
玩家 ↔ 敌人:OnEnemyBeginOverlay
  ├─ 敌人 Normal   → Cast<APacman>(OtherActor)->InJured()
  ├─ 敌人 Vulnerable → SetDead() (回出生点 + 5 秒 CD)
  └─ 敌人 Dead       → 忽略
```

### 数据流 — 受伤与复活

```
APacman::InJured()
  ├─ SetCollisionProfileName("wudi")  ← 3 秒无敌
  ├─ Lifes--
  ├─ Lifes == 0
  │    └─ GameMode->SetCurrentState(EGameOver)
  │         └─ 清空所有敌人 Timer + 玩家 SetMovement(false)
  └─ Lifes > 0
       ├─ SetActorLocation(StartLocation)
       └─ SetTimer(ResettingTimerHandle, this, &Resetting, 3s, false)
            ↓
APacman::Resetting()
  └─ SetCollisionProfileName("Pawn")   ← 恢复碰撞
```

### 数据流 — 暂停状态保存

```
PauseGame():
  ├─ RecordEnemiesTimer()
  │    └─ 遍历敌人,按当前状态记录 VulnerableTimerHandle 剩余时间
  └─ SetCurrentState(EPause)
       └─ SetMovement(false) for each enemy

ResumeGame():
  ├─ RestoreEnemiesTimer()
  │    └─ 重新 SetTimer(... SetNormal, remaining, false)
  └─ SetCurrentState(EPlay)
```

---

## 🧩 关键设计模式

### 1. EGameState 状态机门控输入

所有玩家输入先判断 `GameModeRef->GetCurrentState() == EGameState::EPlay`,其他状态下静默忽略:

```cpp
void APacman::MoveX(float Value) {
    if (GameModeRef->GetCurrentState() == EGameState::EPlay) {
        if (Controller && Value != 0) { ... }
    }
}
```

> 优点:GameMode 是单一事实源,玩家逻辑无需知道"暂停中""GameOver"等具体状态。

### 2. 自定义碰撞 Profile 实现无敌帧

受伤时切换到 `Config/DefaultEngine.ini` 中预先定义的 `"wudi"` Profile(无碰撞),3 秒后切回 `"Pawn"`:

```cpp
void APacman::InJured() {
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("wudi"));
    Lifes--;
    if (Lifes == 0) {
        GameModeRef->SetCurrentState(EGameState::EGameOver);
    } else {
        SetActorLocation(StartLocation);
        GetWorldTimerManager().SetTimer(ResettingTimerHandle, this,
            &APacman::Resetting, wuditime, false);
    }
}

void APacman::Resetting() {
    GetWorldTimerManager().ClearTimer(ResettingTimerHandle);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
}
```

> 优点:不靠 `bIsInvulnerable` 标志位判断,避免和敌人 AI 的状态机逻辑耦合。

### 3. 敌人三态机(状态 + Timer 联动)

`bIsDead` 与 `bIsVulnerable` 是互斥的两个 bool,但都通过 `SetMovement` 决定 `MaxWalkSpeed`:

```cpp
void APacmanEnemy::SetMovement(bool bCanMove) {
    if (bCanMove) {
        if (!bIsVulnerable && !bIsDead)       MaxWalkSpeed = NormalSpeed;     // 300
        else if (bIsVulnerable && !bIsDead)   MaxWalkSpeed = VulnerableSpeed; // 100
        else if (bIsDead)                     MaxWalkSpeed = 0;
    } else {
        MaxWalkSpeed = 0;
    }
}
```

进入 Vulnerable / Dead 时启动一个 Timer,回调 `SetNormal()` 重置两个 bool。

### 4. NavMesh 随机巡逻 + 移动完成回调链

不写 BehaviorTree,直接用 `OnMoveCompleted` 实现"到达 → 再选点 → 再走"的循环:

```cpp
void AEnemyController::OnMoveCompleted(FAIRequestID, const FPathFollowingResult&) {
    Super::OnMoveCompleted(...);
    GotoNewDestination();
}

void AEnemyController::GotoNewDestination() {
    UNavigationSystemV1* NavMesh = UNavigationSystemV1::GetCurrent(this);
    if (NavMesh) {
        FNavLocation RandomLocation;
        if (NavMesh->GetRandomReachablePointInRadius(
                ControlledEnemy->GetActorLocation(), 10000.f, RandomLocation)) {
            MoveToLocation(RandomLocation.Location, 50.f);
        }
    }
}
```

> 优点:零 AI 配置成本,适合最简 demo;代价是无法表达"追击玩家""逃跑"等意图。

### 5. 暂停时保存敌人计时器剩余值

```cpp
void AMyGameModeBase::RecordEnemiesTimer() {
    for (auto Iter(Enemies.CreateConstIterator()); Iter; ++Iter) {
        if ((*Iter)->bIsVulnerable && !(*Iter)->bIsDead)
            EnemiesTimerRemaining.Add(
                GetWorldTimerManager().GetTimerRemaining((*Iter)->VulnerableTimerHandle));
        else if ((*Iter)->bIsDead)
            EnemiesTimerRemaining.Add(
                GetWorldTimerManager().GetTimerRemaining((*Iter)->VulnerableTimerHandle));
        else
            EnemiesTimerRemaining.Add(-1);
    }
}
```

恢复时用 `SetVulnerable(true, Remaining)` 重设,带 `bRestoreTimer` 标记避免重复触发状态切换逻辑。

### 6. TActorIterator 收集关卡内所有 Actor

`BeginPlay` 时用 `TActorIterator<>` 一次性收集所有 `Apacdot`(用于计数)与所有 `APacmanEnemy`(用于暂停/状态广播):

```cpp
for (TActorIterator<Apacdot> i(GetWorld()); i; ++i) PacdotNum++;
for (TActorIterator<APacmanEnemy> i(GetWorld()); i; ++i) Enemies.Add(Cast<APacmanEnemy>(*i));
```

> 优点:无需手动维护"敌人列表",关卡里放多少只幽灵都会被自动接管。

---

## 🛠 开发指南

### 如何调整敌人 AI 行为

1. **让敌人追击玩家**:把 `GotoNewDestination` 改为 `MoveToActor(GetPlayer())`,或用 BehaviorTree
2. **调整巡逻范围**:修改 `SearchRadius = 10000.0f`(单位:cm)
3. **按颜色差异化**:在 `APacmanEnemy` 加 `EEnemyColor` 枚举,不同颜色不同 `SearchRadius` / `NormalSpeed`

### 如何添加新的豆子类型

1. 在 `Apacdot` 增加属性,例如 `bool bIsCherryPellet`(可加速玩家 5 秒)
2. 在 `APacman::OnPacmanBeginOverlay` 的 `if(CurrentPacdot->bIsSuperPacdot)` 旁追加新分支
3. 复制 `BP_pacdot` 创建新蓝图,在细节面板勾选新字段
4. 在地图中放若干个

### 如何接入 Sound / UI

1. **BGM**:`Content/Audios/BGM.uasset` 已就绪,在 GameMode 或 PlayerController 调 `UGameplayStatics::CreateSound2D`
2. **WBP_ManUI**:打开 `Content/BP/WBP_ManUI.uasset`,用 `BindWidget` 绑 UMG 控件,在 `AMyGameModeBase::SetCurrentState` 各分支中调 `CreateWidget` 并 `AddToViewport`

### 调试小贴士

- 按 `P` 在地图视图中可视化 NavMesh(必须先在 RecastNavMesh actor 上 Build)
- 控制台命令 `displayall PlayerState`、`Pawn` 可查看状态
- `GameplayDebugger`(`'`)选中敌人可见 `bIsDead` / `bIsVulnerable` 实时状态
- 暂停时输入 `Pause` 控制台命令会再次触发 `PauseGame()` 的恢复分支

---

## 🐞 已知问题与 TODO

- [ ] **暂停时未清空 `ResettingTimerHandle`** — 玩家无敌帧在暂停期间仍计时,可能导致恢复后已无无敌
- [ ] **能量豆被吃后 `bIsSuperPacdot` 没传给 UI** — 视觉效果建议在 UI 上显示"恐慌时间"倒计时
- [ ] **AI 太傻** — 4 只幽灵完全随机巡逻,无法追逐玩家或围堵;可改用 BehaviorTree + EQSPick
- [ ] **`SetNormal` 时 `SetMovement(true)` 多余** — 当前已在 `SetNormal` 里手动调用,但 `SetVulnerable/SetDead` 内部也会调,存在重复
- [ ] **GameOver 后 `ResetLevel` 会重置 Lifes,但没有"游戏结束画面"延迟**(直接在帧末切到主菜单或 GameOver UI)
- [ ] **没有音频** — `BGM.uasset` 已导入但 `MyGameModeBase` 未创建 Sound2D 引用
- [ ] **`MyPacdot.uasset` 用途不明** — 与 `BP_pacdot.uasset` 命名相似,可能是能量豆的备用实例
- [ ] **玩家死亡后输入还会让 `CapsuleComponent` 转 Profile** — `InJured` 时 Lifes==0 分支没切 Profile,可能下一帧被敌人重叠

---

## 📜 许可证

本项目仅供学习与个人作品展示使用。

- **代码部分**(`Source/`)可参考使用,商用请自行评估。
- **美术/音频资源**(`Content/`)如需商用请联系原作者。
- **Unreal Engine** 受 Epic Games EULA 约束。

---

## 🙏 致谢

- 🎮 [Unreal Engine](https://www.unrealengine.com/) — Epic Games
- 🟡 经典玩法灵感 — [Pac-Man](https://www.bandainamcoentertainment.com/) (Bandai Namco)
- 🧭 寻路系统 — [Recast & Detour](https://recastnavdetour.org/)
- 📚 感谢所有开源 UE 学习社区

---

<p align="center">
  Made with ❤️ and 🟡
</p>