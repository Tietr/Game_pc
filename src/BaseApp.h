#pragma once
#include <SDL2/SDL.h>
#include <iostream>

class BaseApp {
public:
    BaseApp(const char* title, int width, int height): m_title(title), m_width(width), m_height(height) {}
    virtual ~BaseApp() { cleanup(); }
    int run() {
      if (!init()) {
        std::cerr << "初始化失败" << std::endl;
        return -1;
      }
      m_running = true;
      m_lastTime = SDL_GetPerformanceCounter();
      uint64_t frequency = SDL_GetPerformanceFrequency();

      SDL_Event event;
      while (m_running) {
        // dt时间
        uint64_t currentTime = SDL_GetPerformanceCounter();
        float dt = (currentTime - m_lastTime) / (float)frequency;
        m_lastTime = currentTime;
        // 处理事件
        while (SDL_PollEvent(&event)) {
          handleEvents(event);
          onEvent(event);
        }
        // 更新逻辑
        onUpdate(dt);

        // 绘制
        SDL_RenderClear(m_renderer);
        onRender(m_renderer);
        SDL_RenderPresent(m_renderer);

        //限制帧数
      
      }
      return 0;
    }

protected:
  // 子类可以重写这些钩子函数
  virtual void onInit() {}
  virtual void onUpdate(float deltaTime) {}
  virtual void onRender(SDL_Renderer *renderer) {}
  virtual void onEvent(const SDL_Event &event) {}

  // 获取渲染器，方便子类调用
  SDL_Renderer *getRenderer() { return m_renderer; }

private:
  const char *m_title;
  int m_width, m_height;
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;
  bool m_running = false;
  uint64_t m_lastTime = 0;


  bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      std::cerr << "SDL_Init 失败: " << SDL_GetError() << std::endl;
      return false;
    }

    m_window =
        SDL_CreateWindow("SDL2 测试 - 按ESC退出", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
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