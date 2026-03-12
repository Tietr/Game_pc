#pragma once
#include "BaseApp.h"
#include "SceneManager.h"
#include "SceneUIManager.h"
#include "SerialManager.h"
#include <array>
#include <chrono>
#include <deque>
#include <memory>

// 游戏状态枚举
enum class GameState { Running, Flashing };

// 命中检测系统状态机（简化版）
enum class HitDetectionState {
  Idle,        // 空闲状态：正常渲染和逻辑
  FirePrepare, // 准备状态：获取基准线等数据
  Fire         // 开火状态：直接检测光照数值并判断命中
};

class App : public BaseApp {
public:
  App(const char *title, int width, int height);
  ~App();
  virtual void onInit() override;
  virtual void onUpdate(float deltaTime) override;
  virtual void onRender(SDL_Renderer *renderer, TTF_Font *font) override;
  virtual void onEvent(const SDL_Event &event) override;

private:
  // 命中检测系统（简化版）
  HitDetectionState m_hitState = HitDetectionState::Idle;
  float m_hitStateTimer = 0.0f;
  int m_hitThreshold = 30;         // 命中阈值（亮度增加的最小值）
  int m_currentFlashingIndex = -1; // 当前闪烁的UI索引

  // 亮度基准管理
  std::deque<int> m_brightnessHistory;
  int m_currentBaseline = 0;
  static const size_t BASELINE_WINDOW = 50;

  // 串口状态
  enum class SerialState { Connected, Disconnected };
  SerialState m_serialCurrentState = SerialState::Disconnected;
  SerialState m_serialLastState = SerialState::Disconnected;
  std::unique_ptr<SceneManager> m_sceneManager;
  std::unique_ptr<SerialManager> m_serialManager;

  // 闪烁相关状态（保留兼容性）
  GameState m_gameState = GameState::Running;
  float m_flashTimer = 0.0f;

  // USB数据缓冲区（保留兼容性）
  struct Data {
    bool IsDown = false;
    uint32_t Value = 0;
    uint32_t Percent = 0;
  };
  std::array<std::optional<Data>, 100> m_dataBuffer;

  // 命中检测方法（简化版）
  void updateHitDetection(float deltaTime);
  void processSerialData(const std::string &data);
  void handleHardwareMessage(const std::string &message);
  void startFlashing();
  void stopFlashing();
  void processHit();
  void updateBaseline(int brightness);
  int extractBrightness(const std::string &data);

  // 获取当前场景的UIManager
  std::shared_ptr<SceneUIManager> getCurrentUIManager();
};