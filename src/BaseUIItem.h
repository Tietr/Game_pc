#pragma once
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "DrawUtils.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include <string>


enum class RenderType { Render_None, Render_Flash, Render_Normal };

struct Item {
  SDL_Rect m_rect;
  std::string m_text;
  SDL_Color m_color;
  const float m_flashDuration;
  Item(const SDL_Rect &rect, const std::string &text, const SDL_Color &color,
       float flashDuration)
      : m_rect(rect), m_text(text), m_color(color),
        m_flashDuration(flashDuration) {
    m_renderType = RenderType::Render_Normal;
  }
  ~Item() = default;

  void ChangeRenderType(RenderType type) {
    if (m_renderType == RenderType::Render_Flash) { // 防止重复修改
      return;
    }
    m_renderType = type;
  }
  RenderType GetRenderType() const { return m_renderType; }
  RenderType Update(float deltaTime) {
    if (RenderType::Render_None == m_renderType) {
      m_flashTimer = 0.f;
      return m_renderType;
    } else if (RenderType::Render_Normal == m_renderType) {
      m_flashTimer = 0.f;
      return m_renderType;
    } else {
      if (m_flashTimer >= m_flashDuration) {
        m_renderType = RenderType::Render_None;
      }
      m_flashTimer += deltaTime;
      return m_renderType;
    }
  }
  void Render(SDL_Renderer *render, TTF_Font *font) {
    if (RenderType::Render_None == m_renderType) {
      return;
    } else if (RenderType::Render_Normal == m_renderType) {
      NormalRender(render, font);
    } else {
      FlashRender(render);
    }
  }

private:
  float m_flashTimer = 0.0f;
  RenderType m_renderType = RenderType::Render_None;
  void NormalRender(SDL_Renderer *render, TTF_Font *font) {
    DrawUtils::FillRectWithText(render, font, m_rect, m_color, m_text.c_str());
  }
  void FlashRender(SDL_Renderer *render) {
    // SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    // SDL_RenderFillRect(render, &m_rect);
    SDL_Color color = {255, 255, 255, 255};
    DrawUtils::FillRext(render, m_rect, color);
  }

};