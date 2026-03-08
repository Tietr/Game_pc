#pragma once
#include <string>
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
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
  virtual void Render(SDL_Renderer *render) = 0;
  virtual void HandleInput(const SDL_Event &event) = 0;
  inline virtual std::string GetName() const { return m_name; }
  inline virtual std::string GetNextSceneName() const {
    return m_nextSceneName;
  }

protected:
  std::string m_name = "BaseScene";
  std::string m_nextSceneName = "BaseScene";
};