#include "App.h"
#include "GameScene.h"
#include "MainMenuScene.h"

App::App(const char *title, int width, int height)
    : BaseApp(title, width, height) {

  m_sceneManager = std::make_unique<SceneManager>();
  m_serialManager = std::make_unique<SerialManager>();

  m_sceneManager->RegisterScene("game", std::make_shared<GameScene>());
  m_sceneManager->RegisterScene("mainMenu", std::make_shared<MainMenuScene>());
  m_sceneManager->ChangeScene("mainMenu");

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
  m_sceneManager->Render(renderer, font);
  SDL_RenderPresent(renderer);
}

// 更新逻辑and动画
void App::onUpdate(float deltaTime) {
  analyzeUSBData();
  m_sceneManager->Update(deltaTime);

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
void App::drawNotify(SDL_Renderer *renderer, TTF_Font *font,
                     const std::string &text) {
  // 右上角绘制连接状态
  SDL_Color color = {255, 255, 255};
  SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
  if (!textSurface) {
    return;
  }
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(renderer, textSurface);
  if (textTexture) {
    SDL_Rect dstRect = {500, 0, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
    SDL_DestroyTexture(textTexture);
  }
  SDL_FreeSurface(textSurface);
}
