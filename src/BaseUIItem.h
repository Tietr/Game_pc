#pragma once
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "DrawUtils.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_ttf.h"

enum class RenderType { Render_None, Render_Flash, Render_Normal };

class BaseUIItem {
public:
  BaseUIItem(const SDL_Rect &rect, float flashDuration, bool canFlash)
      : m_rect(rect), m_flashDuration(flashDuration),
        m_renderType(RenderType::Render_Normal), m_canFlash(canFlash) {}

  virtual ~BaseUIItem() = default;
  void ChangeRenderType(RenderType type) {
    if (m_renderType == RenderType::Render_Flash &&
        type != RenderType::Render_None) {
      return;
    }
    m_renderType = type;
  }

  RenderType GetRenderType() const { return m_renderType; }
  bool CanFlash() const { return m_canFlash; }
  RenderType Update(float deltaTime) {
    if (m_renderType == RenderType::Render_Flash) {
      m_flashTimer += deltaTime;
      if (m_flashTimer >= m_flashDuration) {
        m_renderType = RenderType::Render_None;
        m_flashTimer = 0.0f;
      }
    } else {
      m_flashTimer = 0.0f;
      OnNormalUpdate(deltaTime);
    }
    return m_renderType;
  }

  // 渲染入口：模板方法模式
  void Render(SDL_Renderer *render, TTF_Font *font) {
    if (m_renderType == RenderType::Render_None)
      return;

    if (m_renderType == RenderType::Render_Normal) {
      // 调用勾子函数，让子类决定怎么画
      OnRenderNormal(render, font);
    } else if (m_renderType == RenderType::Render_Flash) {
      // 闪烁效果通常是通用的（白色填充），也可以设为虚函数
      OnRenderFlash(render);
    }
  }

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
  RenderType m_renderType;
  bool m_canFlash;
};
