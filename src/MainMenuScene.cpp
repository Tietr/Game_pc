#include "MainMenuScene.h"
#include "UIItem.h"
#include <iostream>

void MainMenuScene::OnInit(const std::string &name) {
  BaseScene::OnInit(name);
  m_uiManager = std::make_shared<SceneUIManager>();
  SDL_Rect button_RUN_Rect = {100, 100, 200, 100};
  SDL_Colour button_RUN_Color = {0, 255, 0};
  SDL_Rect button_QUIT_Rect = {100, 300, 200, 100};
  SDL_Colour button_QUIT_Color = {255, 0, 0};
  float flashDuration = 1.0f;
  bool canFlash = true;

  m_uiManager->AddItem(std::make_unique<ButtonWithText>(
      button_RUN_Rect, flashDuration, canFlash, "RUN", button_RUN_Color));
  m_uiManager->AddItem(std::make_unique<ButtonWithText>(
      button_QUIT_Rect, flashDuration, canFlash, "QUIT", button_QUIT_Color));
}
void MainMenuScene::HandleInput(const SDL_Event &event) {
  switch (event.type) {
  case SDL_QUIT:
    m_nextSceneName = "";
    break;
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {
    case SDLK_ESCAPE:
      m_nextSceneName = "quit";
      break;
    case SDLK_RETURN:
      m_nextSceneName = "game";
      break;
    case SDLK_SPACE:
      m_uiManager->EnableFlash();
      break;
    }
  default:
    break;
  }
}
void MainMenuScene::OnEnter() {
  BaseScene::OnEnter();
  std::cout << "Entering " << m_name << std::endl;
}
void MainMenuScene::OnExit() { std::cout << "Exiting " << m_name << std::endl; }
void MainMenuScene::Render(SDL_Renderer *render, TTF_Font *font) {
  SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
  SDL_RenderClear(render);
  m_uiManager->Render(render, font);
}
void MainMenuScene::Update(float deltaTime) { m_uiManager->Update(deltaTime); }
