# GameScene 更新说明

## 更新概述

已将 `GameScene` 从使用旧的 `Background` 和 `Duck` 类迁移到使用新的Anim类系统，通过 `SceneUIManager` 统一管理所有UIItem。

## 主要修改

### 1. GameScene.h 修改
- **移除了旧引用**: 移除了 `Background.h` 和 `Duck.h` 的引用
- **添加了新引用**: 添加了 `SceneUIManager.h` 和 `UIItem.h` 的引用
- **更新了成员变量**:
  - 移除了 `std::unique_ptr<Background> m_background`
  - 移除了 `std::vector<std::unique_ptr<Duck>> m_ducks`
  - 添加了 `std::unique_ptr<SceneUIManager> m_uiManager`
  - 添加了 `std::vector<AimDuck*> m_duckPointers`（用于后期管理）
- **添加了新的方法声明**:
  - `CreateAnimBackground()`: 创建背景
  - `CreateAnimClouds()`: 创建云朵
  - `CreateAimDucks()`: 创建鸭子
  - `AddDuck()`, `RemoveDuck()`, `ClearAllDucks()`: 鸭子管理函数

### 2. GameScene.cpp 修改
- **更新了头文件**: 移除了旧的头文件，添加了新的头文件
- **重写了 OnInit() 方法**:
  - 初始化 `SceneUIManager`
  - 按顺序创建Anim对象
  - 添加了随机数种子初始化
- **实现了创建方法**:
  - `CreateAnimBackground()`: 创建全屏背景，不可闪烁
  - `CreateAnimClouds()`: 创建三朵不同速度的云朵，使用穿梭模式
  - `CreateAimDucks()`: 创建三只可闪烁的鸭子，使用反弹模式
- **更新了 Update() 方法**:
  - 使用 `m_uiManager->Update()` 更新所有UIItem
  - 处理闪烁状态逻辑
- **更新了 Render() 方法**:
  - 使用 `m_uiManager->Render()` 渲染所有UIItem
- **添加了鸭子管理函数**:
  - `AddDuck()`: 在指定位置添加新鸭子
  - `RemoveDuck()`: 移除指定索引的鸭子
  - `ClearAllDucks()`: 清除所有鸭子

## 创建的Anim对象

### 1. AnimBackground（最底层）
- **位置**: 全屏 (0, 0, 800, 600)
- **闪烁**: 不可闪烁 (`flashDuration=0.0f, canFlash=false`)
- **物理**: 无物理组件
- **作用**: 作为背景底层

### 2. AnimCloud × 3（云朵）
- **位置**: 屏幕偏上不同位置
  - 左边: (100, 50)
  - 中间: (350, 80) 
  - 右边: (600, 60)
- **尺寸**: 80×40 像素
- **闪烁**: 不可闪烁
- **物理**: 穿梭模式 (`WrapMove`)
- **速度**: 不同速度向右移动
  - 左边: 60像素/秒
  - 中间: 100像素/秒
  - 右边: 80像素/秒
- **作用**: 背景动画元素

### 3. AimDuck × 3（鸭子）
- **位置**: 屏幕不同位置
  - 左边: (200, 300)
  - 中间: (400, 250)
  - 右边: (600, 350)
- **尺寸**: 40×40 像素
- **闪烁**: 可闪烁 (`flashDuration=0.5f, canFlash=true`)
- **物理**: 反弹模式 (`BounceMove`)
- **速度**: 不同方向速度
  - 左边: (120, 90) - 右下方向
  - 中间: (-100, 120) - 左上方向
  - 右边: (80, -110) - 右上方向
- **作用**: 游戏主要元素

## 渲染顺序

通过 `SceneUIManager` 管理，按照添加顺序渲染：
1. **AnimBackground**（最先添加，在最底层）
2. **AnimCloud**（云朵，在背景之上）
3. **AimDuck**（鸭子，在最上层）

## 物理行为

### 云朵 (AnimCloud)
- **模式**: 穿梭模式 (`WrapMove`)
- **行为**: 移动到屏幕外时从另一边出现
- **适用**: 循环移动的背景元素

### 鸭子 (AimDuck)  
- **模式**: 反弹模式 (`BounceMove`)
- **行为**: 撞到边界时反弹
- **适用**: 需要碰撞的游戏元素

### 背景 (AnimBackground)
- **模式**: 无物理组件
- **行为**: 位置固定
- **适用**: 静态背景

## 后期扩展功能

### 鸭子动态管理
```cpp
// 添加新鸭子
gameScene.AddDuck(x, y);

// 移除指定鸭子
gameScene.RemoveDuck(index);

// 清除所有鸭子
gameScene.ClearAllDucks();
```

### 闪烁控制
- 按空格键触发闪烁状态
- `SceneUIManager` 会处理所有可闪烁元素的闪烁逻辑

### 物理模式切换
可以通过 `GetPhysics()` 访问物理组件并切换模式：
```cpp
duck.GetPhysics().ChangeToWrapMode();  // 切换到穿梭模式
duck.GetPhysics().ChangeToBounceMode(); // 切换回反弹模式
```

## 与旧系统的兼容性

### 已移除的依赖
- `Background` 类
- `Duck` 类
- 相关的更新和渲染逻辑

### 保持的功能
- 场景管理接口不变
- 输入处理逻辑不变
- 游戏状态管理不变

## 测试验证

1. **编译测试**: 项目编译成功
2. **功能测试**: 通过 `TestGameScene.cpp` 验证基本功能
3. **渲染测试**: 需要实际运行查看渲染效果

## 后续优化建议

1. **性能优化**: 大量Anim对象时的性能考虑
2. **配置化**: 从配置文件加载Anim对象参数
3. **粒子系统**: 添加更复杂的动画效果
4. **碰撞检测**: 添加鸭子之间的碰撞检测
5. **状态保存**: 保存和恢复游戏状态