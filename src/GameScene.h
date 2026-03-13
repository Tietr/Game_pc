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

private:
  enum class GameState { Flashing, Running, None };
  GameState m_gameState = GameState::None;


  // // 存储鸭子指针，方便后期管理
  // std::vector<AimDuck *> m_duckPointers;
};