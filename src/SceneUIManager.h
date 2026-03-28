#pragma once

#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "BaseUIItem.h"
#include <SDL_ttf.h>
#include <memory>
#include <vector>

class SceneUIManager {
public:
  // UI管理器模式
  enum class UIManagerMode {
    NormalMode, // 普通模式：RenderType仅有Normal和None，手动控制渲染类型
    FlashMode // 闪烁模式：RenderType有Normal、Flash和None，启动闪烁后自动切换渲染类型
  };

  SceneUIManager(UIManagerMode mode = UIManagerMode::FlashMode)
      : m_mode(mode) {}
  ~SceneUIManager() = default;
  void AddItem(std::unique_ptr<BaseUIItem> item);
  void EnableFlash();

  void Render(SDL_Renderer *render, TTF_Font *font);

  void Update(const float deltaTime);
  void SetRenderType(RenderState renderType);
  int getCurrentFlashingIndex() const;
  bool isItemFlashing(size_t index) const;
  bool removeItem(size_t index);
  size_t getFlashableCount() const;
  void resetFlashState();
  RenderState GetRenderState() const { return m_renderType; }

private:
  std::vector<std::unique_ptr<BaseUIItem>> m_items;
  RenderState m_renderType = RenderState::Render_Normal;
  UIManagerMode m_mode = UIManagerMode::FlashMode;
  size_t m_itemIndexCanFlash = 0;
  void UpdateNormalLogic(const float &deltaTime);
  void UpdateFlashLogic(const float &deltaTime);
};