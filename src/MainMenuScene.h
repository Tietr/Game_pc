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

  void FlashPos(SDL_Renderer *render, SDL_Rect rect, Uint32 FlashTime) {
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderFillRect(render, &rect);
    SDL_RenderPresent(render);
    SDL_Delay(FlashTime);
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderFillRect(render, &rect);
    SDL_RenderPresent(render);
  }
};