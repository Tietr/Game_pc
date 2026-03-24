#include "App.h"
#include "GameScene.h"
#include "MainMenuScene.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <iostream>
#include <vector>

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
    // 等待足够样本再切换
    if (m_prepareSampleCount >= MIN_PREPARE_SAMPLES) {
      startFlashing();
    }
    break;
  case HitDetectionState::Fire:
    if (m_hitStateTimer >= 2.0f) {
      stopFlashing();
    }
    break;
  }
}

void App::handleHardwareMessage(const std::string &message) {
  if (message.find("FIRE") != std::string::npos) {
    if (m_hitState == HitDetectionState::Idle) {
      // 只有在有有效基准线时才进入FirePrepare状态
      if (m_hasValidBaseline) {
        m_hitState = HitDetectionState::FirePrepare;
        m_hitStateTimer = 0.0f;
        m_prepareSampleCount = 0;
        std::cout << "收到FIRE信号，进入FirePrepare状态" << std::endl;
        std::cout << "当前基准线: " << m_currentBaseline << std::endl;
      } else {
        std::cout << "收到FIRE信号，但无有效基准线，忽略" << std::endl;
      }
    }
    return;
  }

  if (message.find("Value:") == std::string::npos) {
    return;
  }

  int brightness = extractBrightness(message);
  std::cout << "当前亮度值: " << brightness << std::endl;

  // Idle状态：直接记录为基准线
  if (m_hitState == HitDetectionState::Idle) {
    m_lastIdleBrightness = brightness;
    m_currentBaseline = brightness;
    m_hasValidBaseline = true;
    std::cout << "Idle状态更新基准线: " << m_currentBaseline << std::endl;
  }

  // FirePrepare状态：等待，不更新基准线
  if (m_hitState == HitDetectionState::FirePrepare) {
    m_prepareSampleCount++;
    std::cout << "准备中: " << m_prepareSampleCount << "/"
              << MIN_PREPARE_SAMPLES << ", 基准线保持: " << m_currentBaseline
              << std::endl;
  }

  // Fire状态：使用Idle状态的基准线检测命中
  if (m_hitState == HitDetectionState::Fire) {
    int delta = brightness - m_currentBaseline;
    std::cout << "Fire状态检测 - 亮度: " << brightness
              << ", 基准线: " << m_currentBaseline << ", Delta: " << delta
              << ", 阈值: " << m_hitThreshold << std::endl;

    // 简单阈值检测
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
  m_prepareSampleCount = 0; // 重置准备计数
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
  m_sceneManager->RemoveFlashUIFromCurrentScene(static_cast<size_t>(hitIndex));
  stopFlashing();
  return;
}

void App::updateBaseline(int brightness) {
  // 简单策略：直接使用传入的亮度值作为基准线
  m_currentBaseline = brightness;
  m_hasValidBaseline = true;
  std::cout << "更新基准线: " << m_currentBaseline << std::endl;
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
