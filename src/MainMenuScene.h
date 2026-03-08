#pragma once
#include "BaseScene.h"
#include "SceneManager.h"

class MainMenuScene : public BaseScene {
public:
  void OnInit(const std::string &name) override;
  void HandleInput(const SDL_Event &event) override;
  void OnEnter() override;
  void OnExit() override;
  void Render(SDL_Renderer *render) override;
  void Update(float deltaTime) override;
};