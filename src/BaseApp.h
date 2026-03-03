#pragma once
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include <SDL.h>
#include <iostream>

class BaseApp {
public:
  BaseApp(const char *title, int width, int height, float fps_Limit = 60.0f,
          float fps_time = 1.0f)
      : m_title(title), m_width(width), m_height(height), FPS_LIMIT(60.0f),
        FPS_TIME(1.0f / FPS_LIMIT) {}
  virtual ~BaseApp() { cleanup(); }
  int run() {
    if (!init()) {
      std::cerr << "初始化失败" << std::endl;
      return -1;
    }
    m_running = true;
    uint64_t m_lastTime = SDL_GetPerformanceCounter();
    uint64_t frequency = SDL_GetPerformanceFrequency();
    float accumulator = 0.0f;

    SDL_Event event;
    while (m_running) {

      uint64_t currentTime = SDL_GetPerformanceCounter();
      float dt = (currentTime - m_lastTime) / (float)frequency;
      m_lastTime = currentTime;
      if (dt > 0.25f) {
        dt = 0.25f; 
      }
      while (SDL_PollEvent(&event)) {
        handleEvents(event);
        onEvent(event);
      }
      
      accumulator += dt * TimeScale;
      while (accumulator >= FPS_TIME) {
        onUpdate(FPS_TIME);
        accumulator -= FPS_TIME;
      }

      SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
      SDL_RenderClear(m_renderer);
      onRender(m_renderer);
      SDL_RenderPresent(m_renderer);
    }
    return 0;
  }

protected:
  // 子类可以重写这些钩子函数
  virtual void onInit() {}
  virtual void onUpdate(float deltaTime) {}
  virtual void onRender(SDL_Renderer *renderer) {}
  virtual void onEvent(const SDL_Event &event) {}

  SDL_Renderer *getRenderer() { return m_renderer; }

  int m_width, m_height;
  float TimeScale = 1.0f;

private:
  const char *m_title;
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;
  bool m_running = false;
  const float FPS_LIMIT;
  const float FPS_TIME;

  bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      std::cerr << "SDL_Init 失败: " << SDL_GetError() << std::endl;
      return false;
    }

    m_window = SDL_CreateWindow("SDL2 测试 - 按ESC退出", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, m_width, m_height,
                                SDL_WINDOW_RESIZABLE);

    if (!m_window) {
      std::cerr << "SDL_CreateWindow 错误: " << SDL_GetError() << std::endl;
      return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr) {
      std::cerr << "SDL_CreateRenderer 失败: " << SDL_GetError() << std::endl;
      return 1;
    }

    onInit();
    return true;
  }
  void handleEvents(const SDL_Event &event) {
    switch (event.type) {
    case SDL_QUIT:
      m_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
        m_running = false;
      break;
    }
  }

  void cleanup() {
    if (m_renderer)
      SDL_DestroyRenderer(m_renderer);
    if (m_window)
      SDL_DestroyWindow(m_window);
    SDL_Quit();
  }
};
