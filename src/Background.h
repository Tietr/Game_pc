#pragma once

#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "SDL_render.h"

#include <vector>

class Background {
private:
  struct Cloud {
    float x, y, scale, speed;
  };
  std::vector<Cloud> m_clouds;

  void DrawCloud(SDL_Renderer *renderer, float x, float y, float scale);
  void DrawCartoonSun(SDL_Renderer *renderer, float x, float y);
  void DrawSky(SDL_Renderer *renderer);
  void DrawGrass(SDL_Renderer *renderer);
  bool m_flash = false;

public:
  Background();
  ~Background() = default;
  void update(float deltaTime);
  void Draw(SDL_Renderer *renderer);
  // 画面黑屏闪烁
  void flashScreen(SDL_Renderer *renderer);
  void enableFlash();
  bool isFlash();
  void disableFlash();
};
