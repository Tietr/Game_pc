#pragma once
#include "BaseScene.h"
#include <iostream>
#include <vector>
#include "Background.h"
#include "Duck.h"
#include "UI.h"
#include "SerialManager.h"

// class Duck;
// class Background;
// class UI;
// class SerialManager;
class GameScene : public BaseScene {
public:
  ~GameScene();
  void OnInit(const std::string_view &name) override;
  void HandleInput(const SDL_Event &event) override;
  void OnEnter() override;
  void OnExit() override;
  void Render(SDL_Renderer *render) override;
  void Update(float deltaTime) override;

private:
// 需要一群鸭子、一个背景、一个UI、一个USB解析和处理数据
// 操作控制类可以暂时不需要
  enum class GameState { Flashing, Running, None };
  GameState m_gameState = GameState::None;
  
  std::vector<std::unique_ptr<Duck>> m_ducks;
  std::unique_ptr<Background> m_background;
  std::unique_ptr<UI> m_ui;

};