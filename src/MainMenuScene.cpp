#include "MainMenuScene.h"

void MainMenuScene::OnInit(const std::string_view &name) {
  m_name = name;
  m_nextSceneName = name;
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
    }
  default:
    break;
  }
}
void MainMenuScene::OnEnter() {
  std::cout << "Entering " << m_name << std::endl;
}
void MainMenuScene::OnExit() { std::cout << "Exiting " << m_name << std::endl; }
void MainMenuScene::Render(SDL_Renderer *render) {
  // 简单给出两个色块上下排布，绿色代表运行，红色代表退出
  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
  SDL_RenderClear(render);
  SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
  SDL_Rect rect1 = {100, 100, 200, 100};
  SDL_RenderFillRect(render, &rect1);
  SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
  SDL_Rect rect2 = {100, 300, 200, 100};
  SDL_RenderFillRect(render, &rect2);
}
void MainMenuScene::Update(float deltaTime) {
  // 什么也不做
}
