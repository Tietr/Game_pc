# 新的物理系统设计说明

## 概述

基于策略模式（Strategy Pattern）重构的物理系统，将边界处理逻辑从固定的"反弹"行为中解耦，提供了更灵活、可扩展的物理组件设计。

## 主要改进

### 1. 核心架构
- **BasePhyMove**: 抽象基类，定义物理移动的基本接口和纯虚函数 `OnHandleBoundary`
- **具体实现类**: 多种边界处理策略
  - `BounceMove`: 反弹模式（默认）
  - `WrapMove`: 穿梭模式（从一边消失，从另一边出现）
  - `ClampMove`: 阻挡模式（撞墙停下）
  - `NoBoundaryMove`: 无边界模式
- **PhyMove**: 兼容性包装类，保持原有接口的同时支持新模式

### 2. 设计优势
- **开闭原则**: 可以添加新的边界处理策略而不修改现有代码
- **运行时动态切换**: 可以在游戏运行时改变物体的物理行为
- **向后兼容**: 现有代码无需修改即可继续使用
- **易于测试**: 每种策略都可以独立测试

## 使用方法

### 基本使用（兼容原有代码）

```cpp
#include "PhyMove.h"

// 1. 创建对象（默认使用反弹模式）
PhyMove physics;

// 2. 设置初始状态
physics.SetPosition(100, 100);
physics.SetVelocity(50, 30);

// 3. 每帧更新
physics.Update(deltaTime, width, height);

// 4. 获取位置用于渲染
float x = physics.GetX();
float y = physics.GetY();
```

### 动态切换模式

```cpp
PhyMove physics;

// 切换到穿梭模式
physics.ChangeToWrapMode();

// 切换到阻挡模式
physics.ChangeToClampMode();

// 切换到无边界模式
physics.ChangeToNoBoundaryMode();

// 切换回反弹模式
physics.ChangeToBounceMode();
```

### 高级用法（直接使用具体类）

```cpp
#include "PhyMoveTypes.h"
#include <memory>

// 使用智能指针管理
std::unique_ptr<BounceMove> bouncePhysics = std::make_unique<BounceMove>();
std::unique_ptr<WrapMove> wrapPhysics = std::make_unique<WrapMove>();

// 运行时切换
class GameEntity {
    std::unique_ptr<BasePhyMove> m_physics;
    
    void ChangeToWrapMode() {
        float x = m_physics->GetX();
        float y = m_physics->GetY();
        float vx = m_physics->GetVX();
        float vy = m_physics->GetVY();
        
        m_physics = std::make_unique<WrapMove>();
        m_physics->SetPosition(x, y);
        m_physics->SetVelocity(vx, vy);
    }
};
```

### 创建自定义边界处理

```cpp
#include "BasePhyMove.h"

class RandomTeleportMove : public BasePhyMove {
protected:
    void OnHandleBoundary(float w, float h) override {
        // 自定义逻辑：碰到左边界时随机传送
        if (m_x < m_minX) {
            m_x = static_cast<float>(rand() % (m_maxX - static_cast<int>(w)));
            m_y = static_cast<float>(rand() % (m_maxY - static_cast<int>(h)));
        }
        // 其他边界使用反弹
        else if (m_x + w > m_maxX) {
            m_x = (float)m_maxX - w;
            m_vx = -std::abs(m_vx);
        }
        // ... 上下边界处理
    }
};
```

## 文件说明

- `BasePhyMove.h`: 物理基类定义
- `PhyMoveTypes.h`: 具体的边界处理策略实现
- `PhyMove.h`: 兼容性包装类（主要使用这个）
- `PhysicsExample.cpp`: 使用示例
- `TestPhysics.cpp`: 测试程序

## 在现有代码中的使用

现有的 `UIItem` 类（`AimDuck`、`AnimCloud`、`AnimBackground`）已经更新为使用新的物理系统。它们通过 `PhyMove` 包装类保持兼容性，同时支持未来的模式切换功能。

## 扩展建议

1. **添加新策略**: 继承 `BasePhyMove` 并实现 `OnHandleBoundary` 方法
2. **配置系统**: 可以从配置文件加载物理行为设置
3. **组合策略**: 可以创建组合多种行为的复合策略
4. **状态模式**: 结合状态模式实现更复杂的状态切换逻辑

## 设计理念

这个重构体现了几个重要的设计原则：
1. **单一职责**: `PhyMove` 只负责移动计算，边界处理由策略类负责
2. **开闭原则**: 可以扩展新的边界行为而不修改现有代码
3. **依赖倒置**: 高层模块不依赖具体实现，而是依赖抽象
4. **策略模式**: 将算法封装在独立的类中，使它们可以相互替换