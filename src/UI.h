#pragma once
#include <SDL_ttf.h>
#include "DrawUtils.h"
class UI {
private:
  int score;
  int maxScore;
  SDL_Color panelColor;

public:
  UI();
  ~UI() = default;

  void addScore();
  

  void reset();

  void draw(SDL_Renderer *renderer);

private:
  void drawMiniDuck(SDL_Renderer *renderer, int x, int y); 
};