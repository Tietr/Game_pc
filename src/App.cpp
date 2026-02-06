#include "App.h"

App::App(const char *title, int width, int height)
    : BaseApp(title, width, height), m_background(), m_ui() {

  m_ducks.push_back(Duck({100, 200, 1.2f}));
  m_ducks.push_back(Duck({400, 250, 1.2f}));
}
App::~App() { std::cout << "Total time: " << m_TotalTime << std::endl; }

// 处理按键逻辑
void App::onEvent(const SDL_Event &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      std::cout << "按下了空格键！" << std::endl;
    }
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    std::cout << "点击了鼠标！" << std::endl;
    // 画面闪烁
    m_background.enableFlash();
  }
}
// 初始化逻辑
void App::onInit() {
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_RenderSetLogicalSize(getRenderer(), 800, 600);
}

// 渲染逻辑
void App::onRender(SDL_Renderer *renderer) {

  m_background.Draw(renderer);
  for (auto &duck : m_ducks) {
    duck.draw(renderer);
  }
  m_ui.draw(renderer);

//   if (m_background.isFlash()) {
//     m_background.flashScreen(renderer);
//     for (auto &duck : m_ducks) {
//       duck.FlashDuck(renderer);
//     }
//     m_background.disableFlash();
//     SDL_RenderPresent(renderer);
//     SDL_Delay(1000);

// }



  SDL_RenderPresent(renderer);
}

// 更新逻辑and动画
void App::onUpdate(float deltaTime) {
  m_TotalTime += deltaTime;
  m_background.update(deltaTime);
  for (auto &duck : m_ducks) {
    duck.update(deltaTime);
  }
}

