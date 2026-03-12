#include "FlashBasedHitDetector.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>


FlashBasedHitDetector::FlashBasedHitDetector(
    std::shared_ptr<SceneUIManager> uiManager,
    std::shared_ptr<SerialManager> serialManager)
    : m_uiManager(uiManager), m_serialManager(serialManager) {}

void FlashBasedHitDetector::update(float deltaTime) {
  // 更新状态计时器
  m_stateTimer += deltaTime;

  // 根据当前状态执行处理
  switch (m_state) {
  case State::Idle:
    handleIdleState(deltaTime);
    break;
  case State::WaitingForShot:
    handleWaitingForShotState(deltaTime);
    break;
  case State::FlashingInProgress:
    handleFlashingInProgressState(deltaTime);
    break;
  case State::DetectingHit:
    handleDetectingHitState(deltaTime);
    break;
  case State::HitConfirmed:
    handleHitConfirmedState(deltaTime);
    break;
  case State::Missed:
    handleMissedState(deltaTime);
    break;
  case State::Error:
    // 错误状态需要手动重置
    break;
  }
}

void FlashBasedHitDetector::processSerialData(const std::string &data) {
  handleHardwareMessage(data);
}

void FlashBasedHitDetector::triggerShot() {
  if (m_state == State::Idle) {
    // 模拟硬件准备信号
    handleHardwareMessage("FIRE_PREPARED");
  }
}

int FlashBasedHitDetector::getCurrentFlashingIndex() const {
  if (!m_uiManager)
    return -1;
  return m_uiManager->getCurrentFlashingIndex();
}

void FlashBasedHitDetector::setConfig(const Config &config) {
  m_config = config;
}

FlashBasedHitDetector::Config FlashBasedHitDetector::getConfig() const {
  return m_config;
}

void FlashBasedHitDetector::reset() {
  m_state = State::Idle;
  m_stateTimer = 0.0f;
  m_flashTimer = 0.0f;
  m_lastHitResult.reset();

  if (m_uiManager) {
    m_uiManager->resetFlashState();
  }
}

const char *FlashBasedHitDetector::stateToString(State state) {
  switch (state) {
  case State::Idle:
    return "Idle";
  case State::WaitingForShot:
    return "WaitingForShot";
  case State::FlashingInProgress:
    return "FlashingInProgress";
  case State::DetectingHit:
    return "DetectingHit";
  case State::HitConfirmed:
    return "HitConfirmed";
  case State::Missed:
    return "Missed";
  case State::Error:
    return "Error";
  default:
    return "Unknown";
  }
}

// ==================== 状态转换 ====================

void FlashBasedHitDetector::transitionTo(State newState) {
  std::cout << "HitDetector: " << stateToString(m_state) << " -> "
            << stateToString(newState) << std::endl;

  // 退出当前状态的清理工作
  switch (m_state) {
  case State::FlashingInProgress:
    stopFlashing();
    break;
  default:
    break;
  }

  // 进入新状态的初始化工作
  m_state = newState;
  m_stateTimer = 0.0f;

  switch (newState) {
  case State::FlashingInProgress:
    startFlashing();
    break;
  case State::DetectingHit:
    // 通知硬件开始检测
    sendToHardware("FLASH_STARTED");
    break;
  default:
    break;
  }
}

// ==================== 状态处理函数 ====================

void FlashBasedHitDetector::handleIdleState(float deltaTime) {
  // 空闲状态：等待硬件信号
  // 可以在这里添加一些视觉效果（如待机动画）
}

void FlashBasedHitDetector::handleWaitingForShotState(float deltaTime) {
  // 等待射击状态：硬件已准备，等待用户射击
  // 可以显示准星或提示信息

  // 超时处理：如果太久没有射击，返回空闲状态
  if (m_stateTimer > 5.0f) {
    transitionTo(State::Idle);
  }
}

void FlashBasedHitDetector::handleFlashingInProgressState(float deltaTime) {
  // UI闪烁中
  m_flashTimer += deltaTime;

  // 检查闪烁是否完成
  if (m_flashTimer >= m_config.flashDuration) {
    transitionTo(State::DetectingHit);
  }
}

void FlashBasedHitDetector::handleDetectingHitState(float deltaTime) {
  // 检测命中状态：等待硬件检测结果

  // 超时处理
  if (m_stateTimer >= m_config.detectionTimeout) {
    processMiss();
  }
}

void FlashBasedHitDetector::handleHitConfirmedState(float deltaTime) {
  // 命中确认状态：短暂显示命中效果

  if (m_stateTimer > 0.5f) { // 显示0.5秒命中效果
    if (m_config.autoAdvance) {
      // 自动切换到下一个UI
      transitionTo(State::FlashingInProgress);
    } else {
      // 等待用户操作
      transitionTo(State::Idle);
    }
  }
}

void FlashBasedHitDetector::handleMissedState(float deltaTime) {
  // 未命中状态：显示未命中效果

  if (m_stateTimer > 0.5f) { // 显示0.5秒未命中效果
    if (m_config.autoAdvance) {
      // 自动切换到下一个UI
      transitionTo(State::FlashingInProgress);
    } else {
      // 等待用户操作
      transitionTo(State::Idle);
    }
  }
}

// ==================== 辅助函数 ====================

void FlashBasedHitDetector::startFlashing() {
  if (!m_uiManager)
    return;

  // 检查是否还有可闪烁的UI
  if (m_uiManager->getFlashableCount() == 0) {
    std::cout << "No flashable UI items available" << std::endl;
    transitionTo(State::Idle);
    return;
  }

  // 启动闪烁
  m_uiManager->EnableFlash();
  m_flashTimer = 0.0f;

  std::cout << "Started flashing UI" << std::endl;
}

void FlashBasedHitDetector::stopFlashing() {
  if (!m_uiManager)
    return;

  // 停止闪烁
  m_uiManager->resetFlashState();
}

void FlashBasedHitDetector::processHit(int deltaValue) {
  if (!m_uiManager)
    return;

  // 获取当前闪烁的UI索引
  int currentIndex = m_uiManager->getCurrentFlashingIndex();
  if (currentIndex < 0) {
    std::cout << "Error: No UI is currently flashing" << std::endl;
    transitionTo(State::Error);
    return;
  }

  // 记录命中结果
  m_lastHitResult = HitResult{currentIndex, deltaValue, true};

  // 命中当前闪烁的UI
  bool hitSuccess = m_uiManager->hitCurrentFlashingItem();

  if (hitSuccess) {
    std::cout << "Hit confirmed! Target index: " << currentIndex
              << ", Delta: " << deltaValue << std::endl;

    // 通知硬件命中确认
    sendToHardware("HIT_CONFIRMED");

    // 切换到命中确认状态
    transitionTo(State::HitConfirmed);
  } else {
    std::cout << "Hit failed - no flashing UI" << std::endl;
    transitionTo(State::Error);
  }
}

void FlashBasedHitDetector::processMiss() {
  std::cout << "Missed!" << std::endl;

  // 通知硬件未命中
  sendToHardware("FLASH_STOPPED");

  // 记录未命中结果
  m_lastHitResult = HitResult{-1, 0, false};

  // 切换到未命中状态
  transitionTo(State::Missed);
}

void FlashBasedHitDetector::sendToHardware(const std::string &message) {
  if (!m_serialManager) {
    std::cout << "Warning: No serial manager, cannot send: " << message
              << std::endl;
    return;
  }

  // 这里需要扩展SerialManager以支持发送数据
  // 暂时只打印日志
  std::cout << "To Hardware: " << message << std::endl;
}

// ==================== 亮度数据处理 ====================

void FlashBasedHitDetector::updateBaseline(int brightness) {
  BrightnessData data{brightness, std::chrono::steady_clock::now()};
  m_baselineBuffer.push_back(data);

  // 保持缓冲区大小
  if (m_baselineBuffer.size() > BASELINE_WINDOW_SIZE) {
    m_baselineBuffer.pop_front();
  }

  // 更新当前基准值
  m_currentBaseline = calculateBaseline();
}

int FlashBasedHitDetector::calculateBaseline() const {
  if (m_baselineBuffer.empty())
    return 0;

  // 计算移动平均
  int sum = 0;
  for (const auto &data : m_baselineBuffer) {
    sum += data.value;
  }

  return sum / static_cast<int>(m_baselineBuffer.size());
}

int FlashBasedHitDetector::extractBrightnessFromData(const std::string &data) {
  // 解析格式: "BRIGHTNESS:428" 或 "Value:428"
  size_t valuePos = data.find("Value:");
  if (valuePos != std::string::npos) {
    size_t start = valuePos + 6;
    size_t end = data.find(",", start);
    if (end == std::string::npos)
      end = data.length();

    std::string valueStr = data.substr(start, end - start);
    try {
      return std::stoi(valueStr);
    } catch (...) {
      return 0;
    }
  }

  size_t brightnessPos = data.find("BRIGHTNESS:");
  if (brightnessPos != std::string::npos) {
    size_t start = brightnessPos + 11;
    std::string valueStr = data.substr(start);
    try {
      return std::stoi(valueStr);
    } catch (...) {
      return 0;
    }
  }

  return 0;
}

int FlashBasedHitDetector::extractDeltaFromData(const std::string &data) {
  // 解析格式: "HIT_DETECTED,Delta:45"
  size_t deltaPos = data.find("Delta:");
  if (deltaPos != std::string::npos) {
    size_t start = deltaPos + 6;
    std::string deltaStr = data.substr(start);
    try {
      return std::stoi(deltaStr);
    } catch (...) {
      return 0;
    }
  }

  return 0;
}

// ==================== 硬件消息处理 ====================

void FlashBasedHitDetector::handleHardwareMessage(const std::string &message) {
  std::cout << "From Hardware: " << message << std::endl;

  // 更新调试信息
  m_debugInfo = message;

  // 根据消息类型处理
  if (message.find("BRIGHTNESS:") != std::string::npos ||
      message.find("Value:") != std::string::npos) {
    // 亮度数据：更新基准
    int brightness = extractBrightnessFromData(message);
    updateBaseline(brightness);
  } else if (message.find("FIRE_PREPARED") != std::string::npos) {
    // 硬件准备开火
    if (m_state == State::Idle) {
      transitionTo(State::FlashingInProgress);
    }
  } else if (message.find("HIT_DETECTED") != std::string::npos) {
    // 硬件检测到命中
    if (m_state == State::DetectingHit) {
      int delta = extractDeltaFromData(message);
      processHit(delta);
    }
  } else if (message.find("MISSED") != std::string::npos) {
    // 硬件报告未命中
    if (m_state == State::DetectingHit) {
      processMiss();
    }
  }
}