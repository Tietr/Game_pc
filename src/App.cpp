#include "App.h"

App::App(const char *title, int width, int height)
    : BaseApp(title, width, height), m_background(), m_ui(),m_serialManager() {



  // 此处鸭子为测试数据
  // m_ducks.push_back(Duck({0,100, 200, 1.2f}));
  // m_ducks.push_back(Duck({1,400, 250, 1.2f}));
}
App::~App() { m_serialManager.stop(); }

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
  m_serialManager.start();
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

    for (Uint32 i = 0; i < m_ducks.size(); i++) {
      m_ducks[i].FlashDuck(renderer,1000);
    }
    m_background.disableFlash();
    SDL_RenderPresent(renderer);
    TimeScale = 1.0f;
  }

  SDL_RenderPresent(renderer);
}

// 更新逻辑and动画
void App::onUpdate(float deltaTime) {
  // 这里的update应该更加通用一写，而不是只是这一个游戏场景的
  // auto inputs = m_serialManager.fetchNewData();
  // for (const auto &msg : inputs) {
  //   // 解析数据，应该是解析按下时间的光照值，以及若干秒后的光照值进行对比，超过多少时候触发
  //     std::cout << "收到数据：" << msg << std::endl;

  // }
  analyzeUSBData();

  
  m_background.update(deltaTime);
  for (auto &duck : m_ducks) {
    duck.updateMove(deltaTime);
  }
}
