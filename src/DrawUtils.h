#pragma once
#include <SDL2/SDL.h>
#include <cmath>

class DrawUtils {
public:
  static void fillCircle(SDL_Renderer *renderer, int cx, int cy, int r,
                         SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);



    for (int dy = -r; dy <= r; dy++) {
      int dx = (int)sqrt(r * r - dy * dy);
      SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
  }

private:
  DrawUtils() = delete;
  
};