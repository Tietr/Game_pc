#pragma once
#include "BaseApp.h"
#include "SceneManager.h"
#include "SerialManager.h"
#include <memory>

#ifdef ENABLE_GUI
#include "implot.h"
#include <vector>


// 环形缓冲区用于绘图
struct ScrollingBuffer {
  int MaxSize;
  int Offset = 0;
  std::vector<double> x, y;
  ScrollingBuffer(int max_size = 1000) : MaxSize(max_size) {
    x.reserve(MaxSize);
    y.reserve(MaxSize);
  }
  void AddPoint(double x_val, double y_val) {
    if (x.size() < MaxSize) {
      x.push_back(x_val);
      y.push_back(y_val);
    } else {
      x[Offset] = x_val;
      y[Offset] = y_val;
      Offset = (Offset + 1) % MaxSize;
    }
  }
};
#endif

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
  virtual void onDebugDraw() override; 
private:
  std::unique_ptr<SerialManager> m_serialManager;
  HitDetectionState m_hitState = HitDetectionState::Idle;
#ifdef ENABLE_GUI
  ScrollingBuffer m_plotData;       
  std::vector<double> m_fireEvents; 
#endif
  bool m_showDebug = true;
  const int m_hitThreshold = 30;         // 命中阈值
  int m_currentFlashingIndex = -1; // 当前闪烁的UI索引

  int m_currentBaseline = 0;
  int m_lastIdleBrightness = 0;    
  bool m_hasValidBaseline = false; 


  size_t m_prepareSampleCount = 0;
  static const size_t MIN_PREPARE_SAMPLES = 2; // 2 * 50ms = 100ms


  void StateMachineUpdate(float deltaTime);
  void handleHardwareMessage(const std::string &message);
  void handleFireSignal();                    
  void handleBrightnessValue(int brightness); 
  void startFlashing();
  void stopFlashing();
  void processHit();
  void updateBaseline(int brightness);
  int extractBrightness(const std::string &data);
  // 获取当前场景的UIManager
  std::unique_ptr<SceneManager> m_sceneManager;
};