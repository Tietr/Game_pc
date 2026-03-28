#pragma once
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "DrawUtils.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_ttf.h"

enum class RenderState { Render_None, Render_Flash, Render_Normal };

class BaseUIItem {
public:
  BaseUIItem(const SDL_Rect &rect, float flashDuration, bool canFlash)
      : m_rect(rect), m_flashDuration(flashDuration),
        m_renderType(RenderState::Render_Normal), m_canFlash(canFlash) {}

  virtual ~BaseUIItem() = default;
  void ChangeRenderType(RenderState type) {
    if (m_renderType == RenderState::Render_Flash &&
        type != RenderState::Render_None) {
      return;
    }
    m_renderType = type;
  }

  RenderState GetRenderType() const { return m_renderType; }
  bool CanFlash() const { return m_canFlash; }
  float GetFlashDuration() const { return m_flashDuration; }
  RenderState Update(float deltaTime) {
    if (m_renderType == RenderState::Render_Flash) {
      m_flashTimer += deltaTime;
      if (m_flashTimer >= m_flashDuration) {
        m_renderType = RenderState::Render_None;
        m_flashTimer = 0.0f;
      }
    } else {
      m_flashTimer = 0.0f;
      OnNormalUpdate(deltaTime);
    }
    return m_renderType;
  }

  const SDL_Rect &GetBounds() const { return m_rect; }
  void SetBounds(const SDL_Rect &newBounds) { m_rect = newBounds; }

  // 渲染入口：模板方法模式
  void Render(SDL_Renderer *render, TTF_Font *font) {
    if (m_renderType == RenderState::Render_None)

      return;
    if (m_renderType == RenderState::Render_Normal) {
      OnRenderNormal(render, font);
    } else if (m_renderType == RenderState::Render_Flash) {
      OnRenderFlash(render);
    }
  }

  virtual void OnDead() {}

protected:
  virtual void OnRenderNormal(SDL_Renderer *render, TTF_Font *font) = 0;
  virtual void OnNormalUpdate(float deltaTime) {
    // UI进行物理更新，比如位置、大小等
  }
  virtual void OnRenderFlash(SDL_Renderer *render) {
    SDL_Color white = {255, 255, 255, 255};
    DrawUtils::FillRext(render, m_rect, white);
  }

protected:
  SDL_Rect m_rect;
  float m_flashDuration;
  float m_flashTimer = 0.0f;
  RenderState m_renderType;
  bool m_canFlash;
};
