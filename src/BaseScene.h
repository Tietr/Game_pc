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
    // 进入场景时，应用延迟的UI增删操作
    applyDelayedUIOperations();
  };
  virtual void OnExit() = 0;
  virtual void Update(float deltaTime) {
    // 检查可闪烁UI管理器的状态
    if (m_flashUIManager) {
      RenderType flashRenderType = m_flashUIManager->GetRenderType();

      // 状态同步逻辑
      if (flashRenderType == RenderType::Render_Flash) {
        // 可闪烁UI管理器为Flash状态时，普通UI管理器设置为None状态
        if (m_normalUIManager) {
          m_normalUIManager->SetRenderType(RenderType::Render_None);
        }
      } else if (flashRenderType == RenderType::Render_Normal) {
        // 可闪烁UI管理器为Normal状态时，普通UI管理器也设置为Normal状态
        if (m_normalUIManager) {
          m_normalUIManager->SetRenderType(RenderType::Render_Normal);
        }
      }

      // 更新可闪烁UI管理器
      m_flashUIManager->Update(deltaTime);
    }

    // 更新普通UI管理器
    if (m_normalUIManager) {
      m_normalUIManager->Update(deltaTime);
    }
  };
  virtual void Render(SDL_Renderer *render, TTF_Font *font) = 0;
  virtual void HandleInput(const SDL_Event &event) = 0;
  inline virtual std::string GetName() const { return m_name; }
  inline virtual std::string GetNextSceneName() const {
    return m_nextSceneName;
  }

  // 获取场景的UIManager（用于命中检测）- 返回可闪烁的UI管理器
  virtual std::shared_ptr<SceneUIManager> GetUIManager() {
    return m_flashUIManager;
  }

  // 获取普通UI管理器
  virtual std::shared_ptr<SceneUIManager> GetNormalUIManager() {
    return m_normalUIManager;
  }

  // 获取可闪烁UI管理器
  virtual std::shared_ptr<SceneUIManager> GetFlashUIManager() {
    return m_flashUIManager;
  }

  // 延迟添加可闪烁UI（只在场景回到normal模式时应用）
  virtual void AddFlashUIDelayed(std::unique_ptr<BaseUIItem> item) {
    m_delayedAddFlashUI.push_back(std::move(item));
  }

  // 延迟移除可闪烁UI（只在场景回到normal模式时应用）
  virtual void RemoveFlashUIDelayed(size_t index) {
    m_delayedRemoveFlashUI.push_back(index);
  }

  // 立即添加普通UI
  virtual void AddNormalUI(std::unique_ptr<BaseUIItem> item) {
    if (m_normalUIManager) {
      m_normalUIManager->AddItem(std::move(item));
    }
  }

  // 立即移除普通UI
  virtual bool RemoveNormalUI(size_t index) {
    if (m_normalUIManager) {
      return m_normalUIManager->removeItem(index);
    }
    return false;
  }

  // 辅助方法：渲染两个UI管理器
  virtual void RenderUIManagers(SDL_Renderer *render, TTF_Font *font) {
    // 先渲染普通UI（背景、云等）
    if (m_normalUIManager) {
      m_normalUIManager->Render(render, font);
    }

    // 再渲染可闪烁UI（鸭子、按钮等）
    if (m_flashUIManager) {
      m_flashUIManager->Render(render, font);
    }
  }

protected:
  std::string m_name = "BaseScene";
  std::string m_nextSceneName = "BaseScene";
  enum GameState { Flashing, Running };
  GameState m_gameState = Running;

  // 两个UI管理器：一个负责普通UI，一个负责可闪烁UI
  std::shared_ptr<SceneUIManager> m_normalUIManager;
  std::shared_ptr<SceneUIManager> m_flashUIManager;

  // 延迟的UI操作
  std::vector<std::unique_ptr<BaseUIItem>> m_delayedAddFlashUI;
  std::vector<size_t> m_delayedRemoveFlashUI;

  // 应用延迟的UI操作
  virtual void applyDelayedUIOperations() {
    // 只有在normal模式且可闪烁UI为空时才添加新的UI
    if (m_gameState == Running && m_flashUIManager &&
        m_flashUIManager->getFlashableCount() == 0) {
      for (auto &item : m_delayedAddFlashUI) {
        m_flashUIManager->AddItem(std::move(item));
      }
      m_delayedAddFlashUI.clear();
    }

    // 应用延迟的移除操作
    if (m_gameState == Running && m_flashUIManager) {
      // 从大到小排序索引，避免移除时索引变化
      std::sort(m_delayedRemoveFlashUI.rbegin(), m_delayedRemoveFlashUI.rend());
      for (size_t index : m_delayedRemoveFlashUI) {
        m_flashUIManager->removeItem(index);
      }
      m_delayedRemoveFlashUI.clear();
    }
  }
};