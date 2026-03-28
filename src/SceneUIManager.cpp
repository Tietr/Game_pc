#include "SceneUIManager.h"
#include <algorithm>

void SceneUIManager::AddItem(std::unique_ptr<BaseUIItem> item) {
  m_items.push_back(std::move(item));
}
void SceneUIManager::EnableFlash() { m_renderType = RenderState::Render_Flash; }
void SceneUIManager::Render(SDL_Renderer *render, TTF_Font *font) { // 直接渲染
  if (m_items.size() == 0)
    return;

  for (auto &item : m_items) {
    item->Render(render, font);
  }
}
void SceneUIManager::Update(const float deltaTime) { // 更新状态
  if (m_items.size() == 0)
    return;

  if (m_mode == UIManagerMode::FlashMode) {
    // FlashMode：执行自动闪烁逻辑
    if (RenderState::Render_Flash == m_renderType) {
      UpdateFlashLogic(deltaTime);
    } else if (RenderState::Render_Normal == m_renderType) {
      UpdateNormalLogic(deltaTime);
    }
  } else {
    // NormalMode：只更新UI项，不执行自动闪烁逻辑
    for (auto &item : m_items) {
      item->Update(deltaTime);
    }
  }
}
void SceneUIManager::UpdateFlashLogic(const float &deltaTime) {
  if (m_itemIndexCanFlash >= m_items.size()) {
    m_renderType = RenderState::Render_Normal;
    return;
  }
  for (size_t i = m_itemIndexCanFlash; i < m_items.size(); ++i) {
    if (i != m_itemIndexCanFlash) {
      m_items[i]->ChangeRenderType(RenderState::Render_None);
      continue;
    }
    if (!m_items[i]->CanFlash()) {
      m_items[i]->ChangeRenderType(RenderState::Render_None);
      ++m_itemIndexCanFlash;
      continue;
    }
    m_items[i]->ChangeRenderType(RenderState::Render_Flash);
    if (m_items[i]->Update(deltaTime) == RenderState::Render_None) {
      ++m_itemIndexCanFlash;
    }
  }
}
void SceneUIManager::UpdateNormalLogic(const float &deltaTime) {
  m_itemIndexCanFlash = 0;
  for (auto &item : m_items) {
    item->ChangeRenderType(RenderState::Render_Normal);
    item->Update(deltaTime);
  }
}
int SceneUIManager::getCurrentFlashingIndex() const {
  if (m_renderType != RenderState::Render_Flash) {
    return -1;
  }

  if (m_itemIndexCanFlash >= m_items.size()) {
    return -1;
  }
  for (size_t i = m_itemIndexCanFlash; i < m_items.size(); ++i) {
    if (m_items[i]->CanFlash() &&
        m_items[i]->GetRenderType() == RenderState::Render_Flash) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

bool SceneUIManager::isItemFlashing(size_t index) const {
  if (index >= m_items.size()) {
    return false;
  }

  if (m_renderType != RenderState::Render_Flash) {
    return false;
  }

  return (index == m_itemIndexCanFlash) && m_items[index]->CanFlash() &&
         m_items[index]->GetRenderType() == RenderState::Render_Flash;
}

bool SceneUIManager::removeItem(size_t index) {
  if (index >= m_items.size()) {
    return false;
  }
  m_items[index]->OnDead();
  m_items.erase(m_items.begin() + index);

  if (index <= m_itemIndexCanFlash && m_itemIndexCanFlash > 0) {
    --m_itemIndexCanFlash;
  }

  return true;
}

size_t SceneUIManager::getFlashableCount() const {
  size_t count = 0;
  for (const auto &item : m_items) {
    if (item->CanFlash()) {
      ++count;
    }
  }
  return count;
}

void SceneUIManager::resetFlashState() {
  if (m_itemIndexCanFlash >= m_items.size()) {
    m_itemIndexCanFlash = 0;
  }

  m_renderType = RenderState::Render_Normal;
  for (auto &item : m_items) {
    item->ChangeRenderType(RenderState::Render_Normal);
  }
}

void SceneUIManager::SetRenderType(RenderState renderType) {
  if (m_mode == UIManagerMode::NormalMode) {
    m_renderType = renderType;
    switch (renderType) {
    case RenderState::Render_Normal:
      for (auto &item : m_items) {
        item->ChangeRenderType(RenderState::Render_Normal);
      }
      break;
    case RenderState::Render_Flash:
      for (auto &item : m_items) {
        item->ChangeRenderType(RenderState::Render_Flash);
      }
      break;
    case RenderState::Render_None:
      for (auto &item : m_items) {
        item->ChangeRenderType(RenderState::Render_None);
      }
      break;
    }
  }
}
