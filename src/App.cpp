#include "App.h"
#include "GameScene.h"
#include "MainMenuScene.h"
#include <algorithm>
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
  m_sceneManager->Update(deltaTime);
  auto data = m_serialManager->fetchNewData();
  if (data) {
    processSerialData(data.value());
  }
  updateHitDetection(deltaTime);
}

// ==================== 命中检测系统实现（简化版） ====================

void App::updateHitDetection(float deltaTime) {
  m_hitStateTimer += deltaTime;

  switch (m_hitState) {
  case HitDetectionState::Idle:
    // 空闲状态：正常渲染和逻辑，等待硬件信号
    // 持续更新基准线
    break;

  case HitDetectionState::FirePrepare:
    // 准备状态：获取基准线等数据
    // 开始闪烁UI，准备检测
    startFlashing();
    m_hitState = HitDetectionState::Fire;
    m_hitStateTimer = 0.0f;
    std::cout << "进入Fire状态，开始检测" << std::endl;
    break;

  case HitDetectionState::Fire:
    // 开火状态：直接检测光照数值并判断命中
    // 检测逻辑在handleHardwareMessage中处理
    // 如果超时（比如2秒）没有检测到命中，则回到Idle状态
    if (m_hitStateTimer >= 2.0f) {
      std::cout << "检测超时，回到Idle状态" << std::endl;
      stopFlashing();
      m_hitState = HitDetectionState::Idle;
      m_hitStateTimer = 0.0f;
    }
    break;
  }
}

void App::processSerialData(const std::string &dataStr) {
  // 处理硬件消息
  handleHardwareMessage(dataStr);
}

void App::handleHardwareMessage(const std::string &message) {
  if (message.find("BRIGHTNESS:") != std::string::npos ||
      message.find("Value:") != std::string::npos) {
    int brightness = extractBrightness(message);
    updateBaseline(brightness);

    // 如果在Fire状态，直接检测亮度变化并判断命中
    if (m_hitState == HitDetectionState::Fire) {
      int delta = brightness - m_currentBaseline;
      std::cout << "Fire状态检测: brightness=" << brightness
                << ", baseline=" << m_currentBaseline << ", delta=" << delta
                << std::endl;

      // 简化判断逻辑：亮度增加超过阈值即为命中
      if (delta > m_hitThreshold) {
        std::cout << "命中! 亮度显著增加 (delta = " << delta << ")"
                  << std::endl;
        processHit();
      }
    }
    return;
  }

  // 硬件发送FIRE信号（按键按下）
  if (message.find("FIRE") != std::string::npos) {
    // 只有在空闲状态才接受新的FIRE信号
    if (m_hitState == HitDetectionState::Idle) {
      m_hitState = HitDetectionState::FirePrepare;
      m_hitStateTimer = 0.0f;
      std::cout << "收到FIRE信号，进入FirePrepare状态" << std::endl;
    }
    return;
  }
}

void App::startFlashing() {
  auto uiManager = getCurrentUIManager();
  if (!uiManager)
    return;

  // 启动闪烁模式
  uiManager->EnableFlash();

  // 获取当前闪烁的UI索引
  m_currentFlashingIndex = uiManager->getCurrentFlashingIndex();

  if (m_currentFlashingIndex >= 0) {
    std::cout << "Started flashing UI index: " << m_currentFlashingIndex
              << std::endl;
  } else {
    std::cout << "No flashable UI items" << std::endl;
    // m_hitState = HitDetectionState::Idle;
  }
}

void App::stopFlashing() {
  auto uiManager = getCurrentUIManager();
  if (!uiManager)
    return;

  uiManager->resetFlashState();
  m_currentFlashingIndex = -1;
}

void App::processHit() {
  auto uiManager = getCurrentUIManager();
  if (!uiManager)
    return;

  // 命中当前闪烁的UI，返回被击中的索引
  int hitIndex = uiManager->hitCurrentFlashingItem();

  if (hitIndex >= 0) {
    std::cout << "命中! 目标索引: " << hitIndex << std::endl;

    // 尝试通过GameScene移除鸭子（保持m_duckPointers同步）
    auto currentScene = m_sceneManager->GetCurrentScene();
    if (currentScene) {
      // 尝试将currentScene转换为GameScene
      auto gameScene = std::dynamic_pointer_cast<GameScene>(currentScene);
      if (gameScene) {
        // 使用GameScene的RemoveDuck方法，它会同步更新m_duckPointers
        gameScene->RemoveDuck(hitIndex);
        std::cout << "通过GameScene移除鸭子，索引: " << hitIndex << std::endl;
      } else {
        // 如果不是GameScene，直接使用UI管理器移除
        bool removed = uiManager->removeItem(static_cast<size_t>(hitIndex));
        if (removed) {
          std::cout << "直接移除UI项，索引: " << hitIndex << std::endl;
        } else {
          std::cout << "移除UI项失败，索引: " << hitIndex << std::endl;
        }
      }
    } else {
      // 没有当前场景，直接使用UI管理器移除
      bool removed = uiManager->removeItem(static_cast<size_t>(hitIndex));
      if (removed) {
        std::cout << "直接移除UI项（无场景），索引: " << hitIndex << std::endl;
      } else {
        std::cout << "移除UI项失败（无场景），索引: " << hitIndex << std::endl;
      }
    }

    // 通知硬件命中确认 HIT_CONFIRMED
    // 停止闪烁，回到Idle状态
    stopFlashing();
    m_hitState = HitDetectionState::Idle;
    m_hitStateTimer = 0.0f;
  } else {
    std::cout << "命中失败 - 没有闪烁的UI" << std::endl;
    stopFlashing();
    m_hitState = HitDetectionState::Idle;
  }
}

void App::updateBaseline(int brightness) {
  m_brightnessHistory.push_back(brightness);
  if (m_brightnessHistory.size() > BASELINE_WINDOW) {
    m_brightnessHistory.pop_front();
  }

  // 计算移动平均
  int sum = 0;
  for (int val : m_brightnessHistory) {
    sum += val;
  }
  m_currentBaseline = sum / static_cast<int>(m_brightnessHistory.size());
}

int App::extractBrightness(const std::string &data) {
  size_t valuePos = data.find("Value:");
  if (valuePos != std::string::npos) {
    size_t start = valuePos + 6;
    size_t end = data.find(",", start);
    if (end == std::string::npos)
      end = data.length();
    std::string valueStr = data.substr(start, end - start);
    valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace),
                   valueStr.end());
    try {
      return std::stoi(valueStr);
    } catch (...) {
      return 0;
    }
  }

  size_t brightnessPos = data.find("BRIGHTNESS:");
  if (brightnessPos != std::string::npos) {
    size_t start = brightnessPos + 11;
    std::string valueStr = data.substr(start);
    try {
      return std::stoi(valueStr);
    } catch (...) {
      return 0;
    }
  }

  return 0;
}

std::shared_ptr<SceneUIManager> App::getCurrentUIManager() {
  auto currentScene = m_sceneManager->GetCurrentScene();
  if (currentScene) {
    return currentScene->GetUIManager();
  }
  return nullptr;
}
