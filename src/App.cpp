#include "App.h"

App::App(const char *title, int width, int height)
    : BaseApp(title, width, height), m_background(), m_ui() {

  m_ducks.push_back(Duck({0,100, 200, 1.2f}));
  m_ducks.push_back(Duck({1,400, 250, 1.2f}));
}
App::~App() {  }

// 处理按键逻辑
void App::onEvent(const SDL_Event &event) {
  // 鼠标点击
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    std::cout << "点击了鼠标！" << std::endl;
    m_background.enableFlash();
    TimeScale = 0.0f;
  }
}
// 初始化逻辑
void App::onInit() {
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_RenderSetLogicalSize(getRenderer(), m_width, m_height);
}

// 渲染逻辑
void App::onRender(SDL_Renderer *renderer) {

  m_background.Draw(renderer);
  for (auto &duck : m_ducks) {
    duck.draw(renderer);
  }
  m_ui.draw(renderer);

  if (m_background.isFlash()) {
    m_background.flashScreen(renderer);

    for (auto &duck : m_ducks) {
      duck.FlashDuck(renderer);
      // 每只鸭子闪烁并延迟
      SDL_Delay(1000);
    }
    {
      m_background.disableFlash();
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
    TimeScale = 1.0f;
  }

  SDL_RenderPresent(renderer);
}

// 更新逻辑and动画
void App::onUpdate(float deltaTime) {
  m_background.update(deltaTime);
  for (auto &duck : m_ducks) {
    duck.updateMove(deltaTime);
  }
}
