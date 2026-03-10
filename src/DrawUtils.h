#pragma once

// 在包含SDL.h之前定义宏来避免 _m_prefetch 冲突
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif

#include "SDL_ttf.h"
#include <SDL.h>

#include <cmath>

class DrawUtils {
public:
  static void fillCircle(SDL_Renderer *renderer, const int cx, const int cy,
                         const int r, const SDL_Color &color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int dy = -r; dy <= r; dy++) {
      int dx = (int)sqrt(r * r - dy * dy);
      SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
  }
  static void FillRectWithText(SDL_Renderer *render, TTF_Font *font,
                               const SDL_Rect &rect, const SDL_Color &bgColor,
                               const char *text) {
    SDL_SetRenderDrawColor(render, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(render, &rect);
    RenderText(render, font, rect, text);
  }

  static void FillRext(SDL_Renderer *render, const SDL_Rect &rect,
                       const SDL_Color &color) {
    SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(render, &rect);
  }

private:
  DrawUtils() = delete;
  static void RenderText(SDL_Renderer *render, TTF_Font *font, SDL_Rect rect,
                         const char *text) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (!textSurface) {
      return;
    }
    SDL_Texture *textTexture =
        SDL_CreateTextureFromSurface(render, textSurface);
    if (textTexture) {
      SDL_RenderCopy(render, textTexture, NULL, &rect);
      SDL_DestroyTexture(textTexture);
    }
    SDL_FreeSurface(textSurface);
  }
};