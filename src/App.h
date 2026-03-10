#pragma once
#include "BaseApp.h"
#include "SceneManager.h"
#include "SerialManager.h"
#include <array>

// 游戏状态枚举
enum class GameState { Running, Flashing };

class App : public BaseApp {
public:
  App(const char *title, int width, int height);
  ~App();
  virtual void onInit() override;
  virtual void onUpdate(float deltaTime) override;
  virtual void onRender(SDL_Renderer *renderer, TTF_Font *font) override;
  virtual void onEvent(const SDL_Event &event) override;

private:
  enum class SerialState { Connected, Disconnected };
  SerialState m_serialCurrentState = SerialState::Disconnected;
  SerialState m_serialLastState = SerialState::Disconnected;
  std::unique_ptr<SceneManager> m_sceneManager;
  std::unique_ptr<SerialManager> m_serialManager;


  // 闪烁相关状态
  GameState m_gameState = GameState::Running;
  float m_flashTimer = 0.0f;
  struct Data {
    bool IsDown = false; // true 表示 Down，false 表示 Up
    uint32_t Value = 0;
    uint32_t Percent = 0;
  };
  // 这个属于游戏的数据，如果满了，则全部丢弃 -- 感觉需要优化，
  std::array<std::optional<Data>, 100> m_dataBuffer;

  void analyzeUSBData();
  void drawNotify(SDL_Renderer *renderer, TTF_Font *font,
                  const std::string &text);
};