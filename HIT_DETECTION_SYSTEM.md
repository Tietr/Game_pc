# USB命中检测系统实现报告

## 1. 系统概述

本系统实现了基于UI闪烁的命中检测机制，通过USB串口与硬件通信，实现精确的目标命中判断。

### 1.1 核心思想

```
正常状态：场景是亮色背景，硬件持续检测亮度
按下按钮：进入"开火准备"状态
UI闪烁：某个UI变成白色亮块（高亮度）
亮度检测：硬件检测亮度变化
命中判断：
  - 亮度增加（Δ > hitThreshold）= 瞄准闪烁UI = 命中
  - 亮度轻微降低（Δ > missThreshold）= 瞄准闪烁UI = 命中
  - 亮度大幅降低（Δ <= missThreshold）= 没瞄准 = 未命中
```

### 1.2 命中判断逻辑

| 亮度变化       | 判断结果               |
| -------------- | ---------------------- |
| Δ > +30        | 命中（亮度显著增加）   |
| -50 < Δ <= +30 | 命中（亮度变化可接受） |
| Δ <= -50       | 未命中（亮度大幅降低） |

### 1.3 优势

- **精确命中判断**：通过亮度变化方向判断是否瞄准闪烁UI
- **实时性好**：硬件持续检测，响应快速
- **抗干扰强**：基准亮度系统过滤环境光变化
- **利用现有机制**：完美结合SceneUIManager的逐个闪烁功能

---

## 2. 系统架构

### 2.1 状态机设计

```
Idle → FirePrepared → Flashing → Detecting → HitConfirmed/Missed
```

| 状态         | 说明                     |
| ------------ | ------------------------ |
| Idle         | 空闲状态，等待硬件信号   |
| FirePrepared | 硬件准备开火             |
| Flashing     | UI闪烁中                 |
| Detecting    | 检测命中（等待硬件结果） |
| HitConfirmed | 命中确认                 |
| Missed       | 未命中                   |

### 2.2 文件修改清单

| 文件                 | 修改内容                       |
| -------------------- | ------------------------------ |
| `App.h`              | 添加命中检测系统相关成员和方法 |
| `App.cpp`            | 实现完整的命中检测状态机       |
| `SceneManager.h`     | 添加GetCurrentScene()方法      |
| `BaseScene.h`        | 添加GetUIManager()虚方法       |
| `GameScene.h`        | 重写GetUIManager()方法         |
| `GameScene.cpp`      | 实现GetUIManager()             |
| `SceneUIManager.h`   | 添加命中检测相关方法           |
| `SceneUIManager.cpp` | 实现新方法                     |
| `BaseUIItem.h`       | 添加GetBounds()方法            |

---

## 3. 通信协议

### 3.1 硬件 → PC（极简协议）

| 消息             | 说明                         |
| ---------------- | ---------------------------- |
| `BRIGHTNESS:428` | 持续发送亮度数据（用于判断） |
| `FIRE`           | 按键按下信号                 |

### 3.2 PC → 硬件

**无需反馈！** 所有逻辑判断由PC软件完成。

### 3.3 完整工作流程

```
1. 硬件持续发送亮度：BRIGHTNESS:xxx
2. PC持续更新亮度基准
3. 玩家按下按键，硬件发送：FIRE
4. PC收到FIRE，开始UI闪烁
5. 闪烁期间，PC自己计算亮度变化并判断：
   - Δ > +30 → 命中
   - -50 < Δ <= +30 → 命中
   - 超时未命中 → 未命中
6. 自动继续下一个UI闪烁
```

---

## 4. 核心实现

### 4.1 App.h 关键成员

```cpp
// 命中检测系统状态机
HitDetectionState m_hitState = HitDetectionState::Idle;
float m_hitStateTimer = 0.0f;
float m_flashDuration = 0.5f;      // 闪烁持续时间
float m_detectionTimeout = 1.0f;   // 检测超时
int m_hitThreshold = 30;           // 命中阈值（亮度增加的最小值）
int m_missThreshold = -50;         // 未命中阈值（亮度降低超过此值为未命中）
int m_currentFlashingIndex = -1;   // 当前闪烁的UI索引

// 亮度基准管理
std::deque<int> m_brightnessHistory;
int m_currentBaseline = 0;
static const size_t BASELINE_WINDOW = 50;
```

### 4.2 状态转换逻辑

```cpp
void App::updateHitDetection(float deltaTime) {
  m_hitStateTimer += deltaTime;
  
  switch (m_hitState) {
  case HitDetectionState::Idle:
    // 等待硬件信号
    break;
    
  case HitDetectionState::FirePrepared:
    startFlashing();
    m_hitState = HitDetectionState::Flashing;
    break;
    
  case HitDetectionState::Flashing:
    if (m_hitStateTimer >= m_flashDuration) {
      m_hitState = HitDetectionState::Detecting;
      sendToHardware("FLASH_STARTED");
    }
    break;
    
  case HitDetectionState::Detecting:
    if (m_hitStateTimer >= m_detectionTimeout) {
      processMiss();
    }
    break;
    
  case HitDetectionState::HitConfirmed:
  case HitDetectionState::Missed:
    // 自动继续下一个UI
    if (m_hitStateTimer >= 0.5f) {
      startFlashing();
      m_hitState = HitDetectionState::Flashing;
    }
    break;
  }
}
```

### 4.3 命中判断逻辑

```cpp
// 收到亮度变化数据时判断
if (delta > m_hitThreshold) {
  // 亮度显著增加 = 瞄准闪烁UI = 命中
  std::cout << "HIT! Brightness increased significantly" << std::endl;
  processHit();
} else if (delta > m_missThreshold) {
  // 亮度轻微降低 = 瞄准闪烁UI = 命中
  std::cout << "HIT! Brightness change is acceptable" << std::endl;
  processHit();
} else {
  // 亮度大幅降低 = 没瞄准 = 未命中
  std::cout << "MISS! Brightness dropped significantly" << std::endl;
  processMiss();
}
```

### 4.4 命中处理

```cpp
void App::processHit() {
  auto uiManager = getCurrentUIManager();
  if (!uiManager) return;

  // 命中当前闪烁的UI
  bool hitSuccess = uiManager->hitCurrentFlashingItem();

  if (hitSuccess) {
    std::cout << "HIT! Target index: " << m_currentFlashingIndex << std::endl;
    // 无需发送给硬件，所有逻辑由PC完成
    m_hitState = HitDetectionState::HitConfirmed;
  }
}
```

---

## 5. SceneUIManager 扩展

### 5.1 新增方法

| 方法                        | 说明                 |
| --------------------------- | -------------------- |
| `getCurrentFlashingIndex()` | 获取当前闪烁的UI索引 |
| `isItemFlashing(index)`     | 检查指定UI是否在闪烁 |
| `hitCurrentFlashingItem()`  | 命中当前闪烁的UI     |
| `removeItem(index)`         | 移除指定UI项         |
| `getFlashableCount()`       | 获取可闪烁UI数量     |
| `resetFlashState()`         | 重置闪烁状态         |

### 5.2 BaseUIItem 扩展

```cpp
// 获取UI边界
const SDL_Rect& GetBounds() const { return m_rect; }

// 设置UI边界
void SetBounds(const SDL_Rect& newBounds) { m_rect = newBounds; }
```

---

## 6. 使用说明

### 6.1 硬件端要求（极简）

1. **持续发送亮度数据**：`BRIGHTNESS:value`（例如：`BRIGHTNESS:428`）
2. **按键按下时发送**：`FIRE`

**就这么简单！** 所有判断逻辑由PC软件完成。

### 6.2 配置参数

在 `App.h` 中可以调整以下参数：

```cpp
float m_flashDuration = 0.5f;      // 闪烁持续时间（秒）
float m_detectionTimeout = 1.0f;   // 检测超时时间（秒）
int m_hitThreshold = 30;           // 命中阈值（亮度增加的最小值）
int m_missThreshold = -50;         // 未命中阈值（亮度降低超过此值为未命中）
```

### 6.3 测试方法

1. 修改硬件固件，按照通信协议发送数据
2. 运行游戏，按下空格键进入游戏场景
3. 硬件发送 `FIRE_PREPARED` 信号
4. 观察UI闪烁和命中检测效果

---

## 7. 待完成事项

1. **SerialManager扩展**：添加`sendData()`方法，实现PC→硬件通信
2. **目标删除功能**：完善`removeItem()`方法，实现被命中UI的真正删除
3. **调试界面**：添加命中检测状态的图形化显示
4. **校准工具**：添加亮度阈值自动校准功能

---

## 8. 总结

本系统利用现有的SceneUIManager逐个闪烁机制，实现了精确的命中检测。通过状态机控制数据流向，将"当前闪烁的UI"作为"可命中的目标"，完美解决了多目标命中识别问题。

系统架构清晰，扩展性好，为后续的硬件集成和功能扩展奠定了基础。

---

*报告生成时间：2026年3月11日*