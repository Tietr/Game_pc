#pragma once
#include <string>
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "SDL_ttf.h"
#include <SDL.h>


class BaseScene {
public:
  ~BaseScene() = default;
  virtual void OnInit(const std::string &name) {
    m_name = name;
    m_nextSceneName = name;
  };
  virtual void OnEnter() { m_nextSceneName = m_name; };
  virtual void OnExit() = 0;
  virtual void Update(float deltaTime) = 0;
  virtual void Render(SDL_Renderer *render, TTF_Font *font) = 0;
  virtual void HandleInput(const SDL_Event &event) = 0;
  inline virtual std::string GetName() const { return m_name; }
  inline virtual std::string GetNextSceneName() const {
    return m_nextSceneName;
  }

protected:
  std::string m_name = "BaseScene";
  std::string m_nextSceneName = "BaseScene";
  void RenderText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                  int x, int y) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (!textSurface) {
      return;
    }
    SDL_Texture *textTexture =
        SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture) {
      SDL_Rect dstRect = {x, y, textSurface->w, textSurface->h};
      SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
      SDL_DestroyTexture(textTexture);
    }
    SDL_FreeSurface(textSurface);
                  }
};