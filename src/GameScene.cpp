#include "GameScene.h"
#include "Background.h"
#include "Duck.h"


void GameScene::Update(float deltaTime) {
  // 检测游戏状态
  // 分析数据
  // 更新背景、鸭子、UI 等信息
  m_background->Update(deltaTime);
  for (auto &duck : m_ducks) {
    duck->updateMove(deltaTime);
  }
}
void GameScene::Render(SDL_Renderer *render) {
  m_background->Draw(render);
  for (auto &duck : m_ducks) {
    duck->draw(render);
  }
  m_ui->draw(render);
  // 闪烁屏幕
}
void GameScene::OnExit() {
  std::cout << "Exiting GameScene" << std::endl;
  m_gameState = GameState::None;
  }
void GameScene::OnEnter() { std::cout << "Entering GameScene" << std::endl; }
void GameScene::HandleInput(const SDL_Event &event) {
  switch (event.type) {
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      m_nextSceneName = "MainMenu";
    }
    if (event.key.keysym.sym == SDLK_SPACE) {
      m_gameState = GameState::Flashing;
    }
    break;
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    std::cout << "Clicked" << std::endl;
  }
}
void GameScene::OnInit(const std::string_view &name) {
  m_name = name;
  m_nextSceneName = name;
  m_background = std::make_unique<Background>();
  m_ui = std::make_unique<UI>();
  m_gameState = GameState::Running;
}
