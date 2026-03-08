#include "App.h"
#include "GameScene.h"
#include "MainMenuScene.h"

App::App(const char *title, int width, int height)
    : BaseApp(title, width, height), m_background(), m_ui() {

  m_sceneManager = std::make_unique<SceneManager>();
  m_serialManager = std::make_unique<SerialManager>();

  m_sceneManager->RegisterScene("game", std::make_shared<GameScene>());
  m_sceneManager->RegisterScene("mainMenu", std::make_shared<MainMenuScene>());
  m_sceneManager->ChangeScene("mainMenu");
  // // 此处鸭子为测试数据
  // m_ducks.push_back(Duck({0,100, 200, 1.2f}));
  // m_ducks.push_back(Duck({1,400, 250, 1.2f}));
}
App::~App() {
  m_serialManager->stop();
  if (m_sceneManager) {
    m_sceneManager.reset();
  }
  if (m_serialManager) {
    m_serialManager.reset();
  }
}

// 处理按键逻辑
void App::onEvent(const SDL_Event &event) {
  // // 闪烁期间忽略所有输入
  // if (m_gameState == GameState::Flashing) {
  //   return;
  // }
  // // 鼠标点击
  // if (event.type == SDL_MOUSEBUTTONDOWN) {
  //   std::cout << "点击了鼠标！" << std::endl;
  //   // 触发闪烁效果
  //   m_gameState = GameState::Flashing;
  //   m_flashTimer = FLASH_DURATION;
  // }

  m_sceneManager->HandleInput(event);
}
// 初始化逻辑
void App::onInit() {
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_RenderSetLogicalSize(getRenderer(), m_width, m_height);
  m_serialManager->start();
}

// 渲染逻辑
void App::onRender(SDL_Renderer *renderer, TTF_Font *font) {
  // // 正常渲染游戏画面
  // m_background.Draw(renderer);
  // for (auto &duck : m_ducks) {
  //   duck.draw(renderer);
  // }
  // m_ui.draw(renderer);

  // // 闪烁效果：黑色遮罩 + 鸭子高亮
  // if (m_gameState == GameState::Flashing) {
  //   // 绘制黑色遮罩
  //   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  //   SDL_RenderFillRect(renderer, nullptr);

  //   // 绘制所有鸭子为白色高亮
  //   for (auto &duck : m_ducks) {
  //     duck.drawFlash(renderer);
  //   }
  // }

  m_sceneManager->Render(renderer, font);
  SDL_RenderPresent(renderer);
}

// 更新逻辑and动画
void App::onUpdate(float deltaTime) { // 闪烁期间暂停游戏更新
  // if (m_gameState == GameState::Flashing) {
  //   m_flashTimer -= deltaTime;
  //   if (m_flashTimer <= 0) {
  //     m_gameState = GameState::Running;
  //   }
  //   return;
  // }

  analyzeUSBData();
  m_sceneManager->Update(deltaTime);

  // m_background.Update(deltaTime);
  // for (auto &duck : m_ducks) {
  //   duck.updateMove(deltaTime);
  // }
}
void App::analyzeUSBData() {
  auto TempData = m_serialManager->fetchNewData();
  if (TempData) {
    std::cout << "Data: " << TempData.value() << std::endl;
  }

  if (TempData) {
    const std::string data = TempData.value();

    // data :Down, Value: 428, Percent:89%
    Data newData;
    if (data.find("Down") != std::string::npos) {
      newData.IsDown = true;
      // 设置为sdl的鼠标点击事件
      SDL_Event event;
      event.type = SDL_MOUSEBUTTONDOWN;
      SDL_PushEvent(&event);

    } else if (data.find("Up") != std::string::npos) {
      newData.IsDown = false;
    }
    size_t valuePos = data.find("Value:");
    if (valuePos != std::string::npos) {
      size_t valueStart = valuePos + 6;
      size_t valueEnd = data.find(",", valueStart);
      if (valueEnd != std::string::npos) {
        std::string valueStr = data.substr(valueStart, valueEnd - valueStart);
        valueStr.erase(
            std::remove_if(valueStr.begin(), valueStr.end(), ::isspace),
            valueStr.end());
        newData.Value = std::stoi(valueStr);
      }
    }
    size_t percentPos = data.find("Percent:");
    if (percentPos != std::string::npos) {
      size_t percentStart = percentPos + 8;
      size_t percentEnd = data.find("%", percentStart);
      if (percentEnd != std::string::npos) {
        std::string percentStr =
            data.substr(percentStart, percentEnd - percentStart);
        percentStr.erase(
            std::remove_if(percentStr.begin(), percentStr.end(), ::isspace),
            percentStr.end());
        newData.Percent = std::stoi(percentStr);
      }
    }
    // 存入缓冲区 - 使用 std::optional 的 has_value() 判断是否为空
    int index = 0;
    while (index < m_dataBuffer.size() && m_dataBuffer[index].has_value()) {
      index++;
    }
    if (index < m_dataBuffer.size()) {
      m_dataBuffer[index] = newData;
    }
  }
}
