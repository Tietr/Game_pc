#pragma once

#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include <SDL_ttf.h>
class UI {
private:
  int score;
  int maxScore;
  SDL_Color panelColor = {0xFF, 0xFF, 0xFF, 0xFF};

public:
  UI();
  ~UI() = default;

  void addScore();

  void reset();

  void draw(SDL_Renderer *renderer);

private:
  void drawMiniDuck(SDL_Renderer *renderer, int x, int y);
};