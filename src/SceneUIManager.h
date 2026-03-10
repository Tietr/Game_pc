#pragma once

#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "BaseUIItem.h"
#include <memory>
#include <SDL_ttf.h>
#include <vector>

class SceneUIManager {
public:
  SceneUIManager() = default;
  ~SceneUIManager() = default;
  void AddItem(std::unique_ptr<BaseUIItem> item);
  void EnableFlash();

  void Render(SDL_Renderer *render, TTF_Font *font);

  void Update(const float deltaTime);

private:
  std::vector<std::unique_ptr<BaseUIItem>> m_items;
  RenderType m_renderType = RenderType::Render_Normal;
  size_t m_itemIndexCanFlash = 0;
  void UpdateNormalLogic(const float &deltaTime);
  void UpdateFlashLogic(const float &deltaTime);
};