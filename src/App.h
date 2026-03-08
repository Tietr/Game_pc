#pragma once
#include "Background.h"
#include "BaseApp.h"
#include "Duck.h"
#include "SerialManager.h"
#include "UI.h"
#include <array>
#include "SceneManager.h"
// 游戏状态枚举
enum class GameState {
  Running, // 正常运行
  Flashing // 闪烁反馈中
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
  std::unique_ptr<SceneManager> m_sceneManager;   // 使用智能指针
  std::unique_ptr<SerialManager> m_serialManager; // 使用智能指针
  Background m_background;
  std::vector<Duck> m_ducks;
  UI m_ui;

  // 闪烁相关状态
  GameState m_gameState = GameState::Running;
  float m_flashTimer = 0.0f;                    // 闪烁剩余时间
  static constexpr float FLASH_DURATION = 1.0f; // 闪烁持续1秒
  // 存储 USB 口接收的数据
  struct Data {
    bool IsDown = false; // true 表示 Down，false 表示 Up
    uint32_t Value = 0;
    uint32_t Percent = 0;
  };
  // 这个属于游戏的数据，如果满了，则全部丢弃
  std::array<std::optional<Data>, 100> m_dataBuffer;

  void analyzeUSBData();
};