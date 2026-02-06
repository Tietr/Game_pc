#pragma once
#include "Background.h"
#include "BaseApp.h"
#include "Duck.h"
#include "UI.h"


class Background;
const double PI = 3.14159265358979323846;
class App : public BaseApp {
public:
  App(const char *title, int width, int height);
  ~App();
  virtual void onInit() override;
  virtual void onUpdate(float deltaTime) override;
  virtual void onRender(SDL_Renderer *renderer) override;
  virtual void onEvent(const SDL_Event &event) override;

private:
  Background m_background;
  std::vector<Duck> m_ducks;
  UI m_ui;

  float m_TotalTime = 0;

};