#pragma once
#include "BaseScene.h"
#include "UIItem.h"
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

  // 鸭子管理函数（供后期扩展）
  void AddDuck(int x, int y);
  void RemoveDuck(int index);
  void ClearAllDucks();

private:
  void CreateAnimBackground();
  void CreateAnimClouds();
  void CreateAimDucks();
  int32_t Score = 0;

private:
  enum class GameState { Flashing, Running, None };
  GameState m_gameState = GameState::None;
  float DuckFloatTimer = 0.2f;
};