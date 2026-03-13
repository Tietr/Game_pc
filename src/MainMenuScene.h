#pragma once
#include "BaseScene.h"


class MainMenuScene : public BaseScene {
public:
  void OnInit(const std::string &name) override;
  void HandleInput(const SDL_Event &event) override;
  void OnEnter() override;
  void OnExit() override;
  void Render(SDL_Renderer *render, TTF_Font *font) override;
  void Update(float deltaTime) override;

private:
  float m_flashingTime = 0.0f;
  float m_flashingTimeDuration = 1.0f;
  // std::shared_ptr<SceneUIManager> m_uiManager;
};