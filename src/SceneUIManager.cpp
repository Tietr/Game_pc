#include "SceneUIManager.h"
#include "DrawUtils.h"
void SceneUIManager::AddItem(std::unique_ptr<BaseUIItem> item) {
  m_items.push_back(std::move(item));
}
void SceneUIManager::EnableFlash() {
  m_renderType = RenderType::Render_Flash;
  m_itemIndexCanFlash = 0;
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
  if (RenderType::Render_Flash == m_renderType) {
    UpdateFlashLogic(deltaTime);
  } else if (RenderType::Render_Normal == m_renderType) {
    UpdateNormalLogic(deltaTime);
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
