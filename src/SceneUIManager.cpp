#include "SceneUIManager.h"
#include <algorithm>

void SceneUIManager::AddItem(std::unique_ptr<BaseUIItem> item) {
  m_items.push_back(std::move(item));
}
void SceneUIManager::EnableFlash() {
  m_renderType = RenderType::Render_Flash;
  m_itemIndexCanFlash = 0;

  // 立即将第一个可闪烁的UI项设置为闪烁状态
  if (!m_items.empty()) {
    for (size_t i = 0; i < m_items.size(); ++i) {
      if (m_items[i]->CanFlash()) {
        m_items[i]->ChangeRenderType(RenderType::Render_Flash);
        break;
      }
    }
  }
}
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
    if (RenderType::Render_Flash == m_renderType) {
      UpdateFlashLogic(deltaTime);
    } else if (RenderType::Render_Normal == m_renderType) {
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
    m_renderType = RenderType::Render_Normal;
    return;
  }
  for (size_t i = m_itemIndexCanFlash; i < m_items.size(); ++i) {
    if (i != m_itemIndexCanFlash) {
      m_items[i]->ChangeRenderType(RenderType::Render_None);
      continue;
    }
    if (!m_items[i]->CanFlash()) {
      m_items[i]->ChangeRenderType(RenderType::Render_None);
      ++m_itemIndexCanFlash;
      continue;
    }
    m_items[i]->ChangeRenderType(RenderType::Render_Flash);
    if (m_items[i]->Update(deltaTime) == RenderType::Render_None) {
      ++m_itemIndexCanFlash;
    }
  }
}
void SceneUIManager::UpdateNormalLogic(const float &deltaTime) {
  m_itemIndexCanFlash = 0;
  for (auto &item : m_items) {
    item->ChangeRenderType(RenderType::Render_Normal);
    item->Update(deltaTime);
  }
}

// --- 新增方法实现 ---

int SceneUIManager::getCurrentFlashingIndex() const {
  if (m_renderType != RenderType::Render_Flash) {
    return -1; // 不在闪烁模式
  }

  if (m_itemIndexCanFlash >= m_items.size()) {
    return -1; // 所有UI都已闪烁完成
  }

  // 找到当前正在闪烁的UI
  // 从m_itemIndexCanFlash开始查找第一个可闪烁且正在闪烁的UI项
  for (size_t i = m_itemIndexCanFlash; i < m_items.size(); ++i) {
    if (m_items[i]->CanFlash() &&
        m_items[i]->GetRenderType() == RenderType::Render_Flash) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

bool SceneUIManager::isItemFlashing(size_t index) const {
  if (index >= m_items.size()) {
    return false;
  }

  if (m_renderType != RenderType::Render_Flash) {
    return false;
  }

  return (index == m_itemIndexCanFlash) && m_items[index]->CanFlash() &&
         m_items[index]->GetRenderType() == RenderType::Render_Flash;
}

std::optional<SDL_Rect> SceneUIManager::getCurrentFlashingBounds() const {
  int flashingIndex = getCurrentFlashingIndex();
  if (flashingIndex < 0) {
    return std::nullopt;
  }

  // 使用BaseUIItem的GetBounds()方法
  return m_items[flashingIndex]->GetBounds();
}

int SceneUIManager::hitCurrentFlashingItem() {
  int currentIndex = getCurrentFlashingIndex();
  if (currentIndex < 0) {
    return -1; // 没有UI在闪烁
  }

  // 立即完成当前UI的闪烁
  if (currentIndex >= 0 && currentIndex < static_cast<int>(m_items.size())) {
    // 强制结束闪烁
    m_items[currentIndex]->ChangeRenderType(RenderType::Render_None);

    // 移动到下一个可闪烁的UI
    ++m_itemIndexCanFlash;

    return currentIndex; // 返回被击中的索引
  }

  return -1;
}

bool SceneUIManager::removeItem(size_t index) {
  if (index >= m_items.size()) {
    return false;
  }

  // 移除指定索引的UI项
  m_items.erase(m_items.begin() + index);

  // 如果移除的是当前或之前的闪烁项，需要调整索引
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

  m_renderType = RenderType::Render_Normal;

  // 重置所有UI的渲染状态
  for (auto &item : m_items) {
    item->ChangeRenderType(RenderType::Render_Normal);
  }
}

void SceneUIManager::SetToNoneState() {
  // 将所有UI项设置为None状态
  for (auto &item : m_items) {
    item->ChangeRenderType(RenderType::Render_None);
  }
}

void SceneUIManager::SetRenderType(RenderType renderType) {
  // 只有在NormalMode下才能手动设置渲染类型
  if (m_mode == UIManagerMode::NormalMode) {
    m_renderType = renderType;

    // 根据新的渲染类型设置所有UI项
    switch (renderType) {
    case RenderType::Render_Normal:
      for (auto &item : m_items) {
        item->ChangeRenderType(RenderType::Render_Normal);
      }
      break;
    case RenderType::Render_Flash:
      // 在NormalMode下，Flash模式意味着所有UI项都闪烁
      for (auto &item : m_items) {
        item->ChangeRenderType(RenderType::Render_Flash);
      }
      break;
    case RenderType::Render_None:
      for (auto &item : m_items) {
        item->ChangeRenderType(RenderType::Render_None);
      }
      break;
    }
  }
}
