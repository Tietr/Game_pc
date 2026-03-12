#pragma once

#include "SceneUIManager.h"
#include "SerialManager.h"
#include <chrono>
#include <deque>
#include <memory>
#include <optional>
#include <queue>


class FlashBasedHitDetector {
public:
  enum class State {
    Idle,               // 空闲状态
    WaitingForShot,     // 等待射击（硬件准备）
    FlashingInProgress, // UI闪烁中
    DetectingHit,       // 检测命中
    HitConfirmed,       // 命中确认
    Missed,             // 未命中
    Error               // 错误状态
  };

  struct Config {
    float flashDuration = 0.5f;    // 单个UI闪烁时间（秒）
    float detectionTimeout = 1.0f; // 检测超时时间
    int hitThreshold = 30;         // 命中阈值（亮度Δ值）
    bool autoAdvance = true;       // 是否自动切换到下一个UI
  };

  FlashBasedHitDetector(std::shared_ptr<SceneUIManager> uiManager,
                        std::shared_ptr<SerialManager> serialManager);

  // 更新状态机
  void update(float deltaTime);

  // 处理USB数据
  void processSerialData(const std::string &data);

  // 手动触发射击（测试用）
  void triggerShot();

  // 获取当前状态
  State getCurrentState() const { return m_state; }

  // 获取当前闪烁的UI索引
  int getCurrentFlashingIndex() const;

  // 配置相关
  void setConfig(const Config &config);
  Config getConfig() const;

  // 重置系统
  void reset();

  // 状态字符串（用于调试）
  static const char *stateToString(State state);

private:
  // 状态转换
  void transitionTo(State newState);

  // 状态处理函数
  void handleIdleState(float deltaTime);
  void handleWaitingForShotState(float deltaTime);
  void handleFlashingInProgressState(float deltaTime);
  void handleDetectingHitState(float deltaTime);
  void handleHitConfirmedState(float deltaTime);
  void handleMissedState(float deltaTime);

  // 辅助函数
  void startFlashing();
  void stopFlashing();
  void processHit(int deltaValue);
  void processMiss();
  void sendToHardware(const std::string &message);

  // 亮度数据处理
  void updateBaseline(int brightness);
  int calculateBaseline() const;
  int extractBrightnessFromData(const std::string &data);
  int extractDeltaFromData(const std::string &data);

  // 硬件消息处理
  void handleHardwareMessage(const std::string &message);

private:
  State m_state = State::Idle;
  Config m_config;

  std::shared_ptr<SceneUIManager> m_uiManager;
  std::shared_ptr<SerialManager> m_serialManager;

  // 计时器
  float m_stateTimer = 0.0f;
  float m_flashTimer = 0.0f;

  // 亮度数据
  struct BrightnessData {
    int value;
    std::chrono::steady_clock::time_point timestamp;
  };

  std::deque<BrightnessData> m_baselineBuffer;
  int m_currentBaseline = 0;
  const size_t BASELINE_WINDOW_SIZE = 50;

  // 命中结果
  struct HitResult {
    int targetIndex = -1;
    int deltaValue = 0;
    bool isHit = false;
  };

  std::optional<HitResult> m_lastHitResult;

  // 调试信息
  std::string m_debugInfo;
};