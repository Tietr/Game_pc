#pragma once
#include "BaseApp.h"
#include "SceneManager.h"
#include "SceneUIManager.h"
#include "SerialManager.h"

#include <deque>
#include <memory>

enum class GameState { Running, Flashing };
enum class HitDetectionState {
  Idle,        // 空闲状态：正常渲染和逻辑
  FirePrepare, // 准备状态：获取基准线等数据
  Fire         // 开火状态：检测光照数值并判断命中
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
  std::unique_ptr<SerialManager> m_serialManager;
  HitDetectionState m_hitState = HitDetectionState::Idle;
  float m_hitStateTimer = 0.0f;
  int m_hitThreshold = 30;         // 命中阈值（亮度增加的最小值）
  int m_currentFlashingIndex = -1; // 当前闪烁的UI索引

  // 亮度基准管理
  std::deque<int> m_brightnessHistory;
  int m_currentBaseline = 0;
  static const size_t BASELINE_WINDOW = 50;

  // 命中检测方法
  void StateMachineUpdate(float deltaTime);
  void handleHardwareMessage(const std::string &message);
  void startFlashing();
  void stopFlashing(); // 作用存疑
  void processHit();
  void updateBaseline(int brightness);
  int extractBrightness(const std::string &data);
  // 获取当前场景的UIManager
  std::unique_ptr<SceneManager> m_sceneManager;
  std::shared_ptr<SceneUIManager> getCurrentSceneUIFlashManager();
};