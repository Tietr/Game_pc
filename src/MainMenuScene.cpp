#include "MainMenuScene.h"
#include <iostream>

void MainMenuScene::OnInit(const std::string &name) { BaseScene::OnInit(name); }
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
  // 简单给出两个色块上下排布，绿色代表运行，红色代表退出
  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
  SDL_RenderClear(render);

  SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
  SDL_Rect rect1 = {100, 100, 200, 100};
  SDL_RenderFillRect(render, &rect1);
  RenderText(render, font, "Run", rect1.x + 50, rect1.y + 50);

  SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
  SDL_Rect rect2 = {100, 300, 200, 100};
  SDL_RenderFillRect(render, &rect2);
  RenderText(render, font, "Quit", rect2.x + 50, rect2.y + 50);

  SDL_RenderPresent(render);
}
void MainMenuScene::Update(float deltaTime) {
  // 什么也不做
}
