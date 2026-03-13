#pragma once
#include <algorithm>
#include <string>

#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "SDL_ttf.h"
#include "SceneUIManager.h"
#include <SDL.h>
#include <memory>
#include <vector>

class BaseScene {
public:
  virtual ~BaseScene() = default;
  virtual void OnInit(const std::string &name) {
    m_name = name;
    m_nextSceneName = name;
    // normalUIManager使用NormalMode（可以手动设置渲染模式）
    m_normalUIManager = std::make_shared<SceneUIManager>(
        SceneUIManager::UIManagerMode::NormalMode);
    // flashUIManager使用FlashMode（自动闪烁逻辑）
    m_flashUIManager = std::make_shared<SceneUIManager>(
        SceneUIManager::UIManagerMode::FlashMode);
  };
  virtual void OnEnter() {
    m_nextSceneName = m_name;
    applyDelayedUIOperations();
  };
  virtual void OnExit() = 0;
  virtual void Update(float deltaTime) {

    applyDelayedUIOperations();
    const RenderType flashRenderType = m_flashUIManager->GetRenderType();
    if (flashRenderType == RenderType::Render_Flash && m_normalUIManager) {
      m_normalUIManager->SetRenderType(RenderType::Render_None);
    } else if (flashRenderType == RenderType::Render_Normal &&
               m_normalUIManager) {
      m_normalUIManager->SetRenderType(RenderType::Render_Normal);
    }
    m_flashUIManager->Update(deltaTime);
    m_normalUIManager->Update(deltaTime);
  };

  virtual void Render(SDL_Renderer *render, TTF_Font *font) = 0;
  virtual void HandleInput(const SDL_Event &event) = 0;
  inline virtual std::string GetName() const { return m_name; }
  inline virtual std::string GetNextSceneName() const {
    return m_nextSceneName;
  }
  virtual std::shared_ptr<SceneUIManager> GetNormalUIManager() {
    return m_normalUIManager;
  }
  virtual int GetCurrentSceneFlashUIIndex() {
    return m_flashUIManager ? m_flashUIManager->getCurrentFlashingIndex() : -1;
  }
  virtual std::shared_ptr<SceneUIManager> GetFlashUIManager() {
    return m_flashUIManager;
  }
  // 增删UI的借口，供给给UIManager使用，确保在当前更新循环结束后才添加或移除UI项
  virtual void AddFlashUIDelayed(std::unique_ptr<BaseUIItem> item) {
    m_delayedAddFlashUI.push_back(std::move(item));
  }
  virtual void RemoveFlashUIDelayed(size_t index) {
    m_delayedRemoveFlashUI.push_back(index);
  }
  virtual void AddNormalUI(std::unique_ptr<BaseUIItem> item) {
    if (m_normalUIManager) {
      m_normalUIManager->AddItem(std::move(item));
    }
  }
  virtual bool RemoveNormalUI(size_t index) {
    if (m_normalUIManager) {
      return m_normalUIManager->removeItem(index);
    }
    return false;
  }
  virtual void RenderUIManagers(SDL_Renderer *render, TTF_Font *font) {
    m_normalUIManager->Render(render, font);
    m_flashUIManager->Render(render, font);
  }

  virtual void EnableFlash() { m_flashUIManager->EnableFlash(); }
  virtual void DisableFlash() { m_flashUIManager->resetFlashState(); }

protected:
  std::string m_name = "BaseScene";
  std::string m_nextSceneName = "BaseScene";
  enum GameState { Flashing, Running };
  GameState m_gameState = Running;
  std::shared_ptr<SceneUIManager> m_normalUIManager;
  std::shared_ptr<SceneUIManager> m_flashUIManager;
  std::vector<std::unique_ptr<BaseUIItem>> m_delayedAddFlashUI;
  std::vector<size_t> m_delayedRemoveFlashUI;
  virtual void applyDelayedUIOperations() {
    if (m_gameState == Running && m_flashUIManager &&
        m_flashUIManager->getFlashableCount() == 0) {
      for (auto &item : m_delayedAddFlashUI) {
        m_flashUIManager->AddItem(std::move(item));
      }
      m_delayedAddFlashUI.clear();
    }
    if (m_gameState == Running && m_flashUIManager) {
      std::sort(m_delayedRemoveFlashUI.rbegin(), m_delayedRemoveFlashUI.rend());
      for (size_t index : m_delayedRemoveFlashUI) {
        m_flashUIManager->removeItem(index);
      }
      m_delayedRemoveFlashUI.clear();
    }
  }
};