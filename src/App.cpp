#include "App.h"
#include "GameScene.h"
#include "MainMenuScene.h"

#include <charconv>
#include <iostream>

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
void App::onEvent(const SDL_Event &event) {
  m_sceneManager->HandleInput(event);
}
void App::onInit() {
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_RenderSetLogicalSize(getRenderer(), m_width, m_height);
  m_serialManager->start();
}
void App::onRender(SDL_Renderer *renderer, TTF_Font *font) {
  m_sceneManager->Render(renderer, font);
  SDL_RenderPresent(renderer);
}
void App::onUpdate(float deltaTime) {
  m_sceneManager->Update(deltaTime);
  auto data = m_serialManager->fetchNewData();
  if (data) {
    handleHardwareMessage(data.value());
  }
  StateMachineUpdate(deltaTime);
}

void App::StateMachineUpdate(float deltaTime) {
  m_hitStateTimer += deltaTime;
  switch (m_hitState) {
  case HitDetectionState::Idle:
    break;
  case HitDetectionState::FirePrepare:
    startFlashing();
    break;
  case HitDetectionState::Fire:
    if (m_hitStateTimer >= 2.0f) {
      stopFlashing();
    }
    break;
  }
}

void App::handleHardwareMessage(const std::string &message) {
  if (message.find("RESET") != std::string::npos) {
    if (m_hitState == HitDetectionState::Idle) {
      m_hitState = HitDetectionState::FirePrepare;
      m_hitStateTimer = 0.0f;
      std::cout << "收到FIRE信号，进入FirePrepare状态" << std::endl;
    }
    return;
  }
  if (message.find("Value:") == std::string::npos) {
    return;
  }
  int brightness = extractBrightness(message);
  updateBaseline(brightness);
  if (m_hitState == HitDetectionState::Fire) {
    int delta = brightness - m_currentBaseline;
    if (delta > m_hitThreshold) {
      processHit();
    }
  }
}

void App::startFlashing() {
  m_sceneManager->EnableFlashForCurrentScene();
  m_hitState = HitDetectionState::Fire;
  m_hitStateTimer = 0.0f;
}

void App::stopFlashing() {
  m_sceneManager->ResetFlashStateForCurrentScene();
  m_hitState = HitDetectionState::Idle;
  m_hitStateTimer = 0.0f;
}

void App::processHit() {
  if (!m_sceneManager) {
    return;
  }
  int hitIndex = m_sceneManager->GetCurrentFlashingUIIndex();
  if (hitIndex < 0) {
    stopFlashing();
    return;
  }
  std::cout << "命中! 目标索引: " << hitIndex << std::endl;
  // TODO：调用Scene的延迟移除方法，确保在当前更新循环结束后才移除UI项，但是此处ai文件极其混乱
  m_sceneManager->RemoveFlashUIFromCurrentScene(static_cast<size_t>(hitIndex));
  stopFlashing();
  return;
}

void App::updateBaseline(int brightness) {
  m_brightnessHistory.push_back(brightness);
  if (m_brightnessHistory.size() > BASELINE_WINDOW) {
    m_brightnessHistory.pop_front();
  }
  int sum = 0;
  for (int val : m_brightnessHistory) {
    sum += val;
  }
  m_currentBaseline = sum / static_cast<int>(m_brightnessHistory.size());
}

int App::extractBrightness(const std::string &data) {
  static const std::string prefix = "Value:";
  if (data.find(prefix) != 0) {
    return 0;
  }
  const char *start = data.data() + prefix.length();
  const char *end = data.data() + data.size();
  int result = 0;
  auto [ptr, ec] = std::from_chars(start, end, result);
  if (ec == std::errc() && (ptr == end || *ptr == '\n' || *ptr == '\r')) {
    return result;
  }
  return 0;
}
// 同样的问题，这个函数应该放在SceneManager里，App不应该直接操作Scene的UI细节
std::shared_ptr<SceneUIManager> App::getCurrentSceneUIFlashManager() {
  auto currentScene = m_sceneManager->GetCurrentScene();
  return currentScene ? currentScene->GetFlashUIManager() : nullptr;
}
