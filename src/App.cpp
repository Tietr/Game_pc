#include "App.h"
#include "GameScene.h"
#include "MainMenuScene.h"

#include <charconv>
#include <iostream>

App::App(const char *title, int width, int height)
    : BaseApp(title, width, height)
#ifdef ENABLE_GUI
      ,m_plotData(1000) // 初始化缓冲区大小
#endif
{
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
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F3) {
    m_showDebug = !m_showDebug; // 按 F3 切换显示/隐藏
  }
  m_sceneManager->HandleInput(event);
}
void App::onInit() {
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_RenderSetLogicalSize(getRenderer(), m_width, m_height);
  m_serialManager->start();
}
void App::onRender(SDL_Renderer *renderer, TTF_Font *font) {
  m_sceneManager->Render(renderer, font);
  // SDL_RenderPresent(renderer);
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
  switch (m_hitState) {
  case HitDetectionState::Idle:
    break;
  case HitDetectionState::FirePrepare:
    if (m_prepareSampleCount >= MIN_PREPARE_SAMPLES) {
      startFlashing();
    }
    break;
  case HitDetectionState::Fire:
    if (!m_sceneManager->IsCurrentSceneFlashing()) {
      stopFlashing();
    }
    break;
  }
}

void App::handleHardwareMessage(const std::string &message) {
  double now = GetGameTime(); // 获取当前游戏时间
  if (message.find("FIRE") != std::string::npos) {
#ifdef ENABLE_GUI
    m_fireEvents.push_back(now); // 记录火警时间点
#endif
    handleFireSignal();
    return;
  }
  if (message.find("Value:") == std::string::npos) {
    return;
  }
  int brightness = extractBrightness(message);
#ifdef ENABLE_GUI
  m_plotData.AddPoint(now, (double)brightness); // 记录亮度数据
#endif

#ifndef NDEBUG
  std::cout << "当前亮度值: " << brightness << std::endl;
#endif
  handleBrightnessValue(brightness);
}

void App::handleFireSignal() {
  if (m_hitState != HitDetectionState::Idle) {
    return;
  }
  if (!m_hasValidBaseline) {
#ifndef NDEBUG
    std::cout << "收到FIRE信号，但无有效基准线，忽略" << std::endl;
#endif
    return;
  }
  m_hitState = HitDetectionState::FirePrepare;

  m_prepareSampleCount = 0;
#ifndef NDEBUG
  std::cout << "收到FIRE信号，进入FirePrepare状态" << std::endl;
  std::cout << "当前基准线: " << m_currentBaseline << std::endl;
#endif
}

void App::handleBrightnessValue(int brightness) {
  switch (m_hitState) {
  case HitDetectionState::Idle:
    m_lastIdleBrightness = brightness;
    m_currentBaseline = brightness;
    m_hasValidBaseline = true;
#ifndef NDEBUG
    std::cout << "Idle状态更新基准线: " << m_currentBaseline << std::endl;
#endif
    break;
  case HitDetectionState::FirePrepare:
    m_prepareSampleCount++;
#ifndef NDEBUG
    std::cout << "准备中: " << m_prepareSampleCount << "/"
              << MIN_PREPARE_SAMPLES << ", 基准线保持: " << m_currentBaseline
              << std::endl;
#endif
    break;
  case HitDetectionState::Fire: {
    int delta = brightness - m_currentBaseline;
#ifndef NDEBUG
    std::cout << "Fire状态检测 - 亮度: " << brightness
              << ", 基准线: " << m_currentBaseline << ", Delta: " << delta
              << ", 阈值: " << m_hitThreshold << std::endl;
#endif
    if (delta > m_hitThreshold) {
      processHit();
    }
    break;
  }
  }
}

void App::startFlashing() {
  m_sceneManager->EnableFlashForCurrentScene();
  m_hitState = HitDetectionState::Fire;
}

void App::stopFlashing() {
  m_sceneManager->ResetFlashStateForCurrentScene();
  m_hitState = HitDetectionState::Idle;
  m_prepareSampleCount = 0;
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

#ifndef NDEBUG
  std::cout << "命中! 目标索引: " << hitIndex << std::endl;
#endif
  m_sceneManager->RemoveFlashUIFromCurrentScene(static_cast<size_t>(hitIndex));
  stopFlashing();
  return;
}

void App::updateBaseline(int brightness) {
  // 简单策略：直接使用传入的亮度值作为基准线
  m_currentBaseline = brightness;
  m_hasValidBaseline = true;
#ifndef NDEBUG
  std::cout << "更新基准线: " << m_currentBaseline << std::endl;
#endif
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

void App::onDebugDraw() {
#ifdef ENABLE_GUI
  if (!m_showDebug) {
    return;
  }

  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  float PAD = 10.0f;
  ImVec2 window_pos =
      ImVec2(viewport->Pos.x + viewport->Size.x - PAD, viewport->Pos.y + PAD);
  ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f); // 右上角对齐

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowBgAlpha(0.2f); // 极高透明度

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoInputs; 

  if (ImGui::Begin("HUD_MONITOR", nullptr, window_flags)) {
    double now = GetGameTime();

    const char *statusStr =
        (m_hitState == HitDetectionState::Idle)
            ? "IDLE"
            : (m_hitState == HitDetectionState::FirePrepare ? "PREP" : "FIRE!");
    ImVec4 statusCol = (m_hitState == HitDetectionState::Fire)
                           ? ImVec4(1, 0, 0, 1)
                           : ImVec4(1, 1, 1, 0.8f);
    ImGui::TextColored(statusCol, "STATUS: %s | V: %d", statusStr,
                       m_lastIdleBrightness);

    if (ImPlot::BeginPlot("##HUDPlot", ImVec2(200, 80),
                          ImPlotFlags_CanvasOnly)) {
      ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations,
                        ImPlotAxisFlags_NoDecorations);
      ImPlot::SetupAxisLimits(ImAxis_X1, now - 5.0f, now, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1024);

      // 绘制亮度线（青色，线宽 1.5）
      if (!m_plotData.x.empty()) {
        ImPlot::SetNextLineStyle(ImVec4(0, 1, 1, 0.8f), 1.5f);
        ImPlot::PlotLine("##B", m_plotData.x.data(), m_plotData.y.data(),
                         (int)m_plotData.x.size(), 0, m_plotData.Offset,
                         sizeof(double));
      }

      // 绘制基准线（暗绿色虚线）
      if (m_hasValidBaseline) {
        double base = (double)m_currentBaseline;
        ImPlot::DragLineY(111, &base, ImVec4(0, 1, 0, 0.4f), 1.0f,
                          ImPlotDragToolFlags_NoInputs);
      }

      // 绘制 FIRE 触发标记（红竖线）
      for (double fireTime : m_fireEvents) {
        if (fireTime > now - 5.0f) {
          ImPlot::DragLineX(333, &fireTime, ImVec4(1, 0, 0, 0.8f), 1.0f,
                            ImPlotDragToolFlags_NoInputs);
        }
      }

      ImPlot::EndPlot();
    }
  }
  ImGui::End();
#endif
}