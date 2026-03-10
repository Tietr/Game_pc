#pragma once


#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "BaseUIItem.h"
#include <SDL_ttf.h>
#include <vector>

class SceneUIManager {
public:
  SceneUIManager() = default;
  ~SceneUIManager() = default;
  void AddItemCanFlash(const bool canFlash, const Item item) {
    m_items.push_back(std::make_pair(item, canFlash));
  }
  void EnableFlash() { m_renderType = RenderType::Render_Flash; }
  void Render(SDL_Renderer *render, TTF_Font *font) { // 直接渲染
    if (m_items.size() == 0)
      return;

    for (auto &item : m_items) {
      item.first.Render(render, font);
    }
  }
  void Update(float deltaTime) { // 更新状态
    if (m_items.size() == 0)
      return;

    if (RenderType::Render_Flash == m_renderType) {
      for (int i = 0; i < m_items.size(); i++) {
        if (i != itemIndexCanFlash) {
          m_items[i].first.ChangeRenderType(RenderType::Render_None);
          m_items[i].first.Update(deltaTime);
          continue;
        }
        if (!m_items[i].second) {
          ++itemIndexCanFlash;
          continue;
        }
        // 启动闪烁
        m_items[i].first.ChangeRenderType(RenderType::Render_Flash);
        if (m_items[i].first.Update(deltaTime) == RenderType::Render_None) {
          ++itemIndexCanFlash;
          continue;
        }
      }
      if (itemIndexCanFlash >= m_items.size() && itemIndexCanFlash != 0 &&
          RenderType::Render_None ==
              m_items[itemIndexCanFlash - 1].first.GetRenderType()) {
        m_renderType = RenderType::Render_Normal;
      }
    } else if (RenderType::Render_Normal == m_renderType) {
      itemIndexCanFlash = 0;
      for (auto &item : m_items) {
        item.first.ChangeRenderType(RenderType::Render_Normal);
        item.first.Update(deltaTime);
      }
    }
  }

private:
  // 界面可以闪烁的元素
  std::vector<std::pair<Item, bool>> m_items;
  RenderType m_renderType = RenderType::Render_Normal;
  int itemIndexCanFlash = 0;
};