#include "App.h"

App::App(const char *title, int width, int height)
    : BaseApp(title, width, height), m_background(), m_ui(), m_serialManager() {

  // 此处鸭子为测试数据
  // m_ducks.push_back(Duck({0,100, 200, 1.2f}));
  // m_ducks.push_back(Duck({1,400, 250, 1.2f}));
}
App::~App() { m_serialManager.stop(); }

// 处理按键逻辑
void App::onEvent(const SDL_Event &event) {
  // 闪烁期间忽略所有输入
  if (m_gameState == GameState::Flashing) {
    return;
  }

  // 鼠标点击
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    std::cout << "点击了鼠标！" << std::endl;
    // 触发闪烁效果
    m_gameState = GameState::Flashing;
    m_flashTimer = FLASH_DURATION;
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
  // 正常渲染游戏画面
  m_background.Draw(renderer);
  for (auto &duck : m_ducks) {
    duck.draw(renderer);
  }
  m_ui.draw(renderer);

  // 闪烁效果：黑色遮罩 + 鸭子高亮
  if (m_gameState == GameState::Flashing) {
    // 绘制黑色遮罩
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, nullptr);

    // 绘制所有鸭子为白色高亮
    for (auto &duck : m_ducks) {
      duck.drawFlash(renderer);
    }
  }

  SDL_RenderPresent(renderer);
}

// 更新逻辑and动画
void App::onUpdate(float deltaTime) { // 闪烁期间暂停游戏更新
  if (m_gameState == GameState::Flashing) {
    m_flashTimer -= deltaTime;
    if (m_flashTimer <= 0) {
      m_gameState = GameState::Running;
    }
    return;
  }
  // 这里的update应该更加通用一写，而不是只是这一个游戏场景的
  // auto inputs = m_serialManager.fetchNewData();
  // for (const auto &msg : inputs) {
  //   //
  //   解析数据，应该是解析按下时间的光照值，以及若干秒后的光照值进行对比，超过多少时候触发
  //     std::cout << "收到数据：" << msg << std::endl;
  // }
  analyzeUSBData();
  m_background.Update(deltaTime);
  for (auto &duck : m_ducks) {
    duck.updateMove(deltaTime);
  }
}
