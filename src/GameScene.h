#pragma once
#include "Background.h"
#include "BaseScene.h"
#include "Duck.h"
// #include "UI.h"
#include <memory>
#include <vector>
class GameScene : public BaseScene {
public:
  ~GameScene() = default;
  void OnInit(const std::string &name) override;
  void HandleInput(const SDL_Event &event) override;
  void OnEnter() override;
  void OnExit() override;
  void Render(SDL_Renderer *render, TTF_Font *font) override;
  void Update(float deltaTime) override;

private:
  enum class GameState { Flashing, Running, None };
  GameState m_gameState = GameState::None;

  std::vector<std::unique_ptr<Duck>> m_ducks;
  std::unique_ptr<Background> m_background;
  // std::unique_ptr<UI> m_ui;
};