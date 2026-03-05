#pragma once
#include "BaseScene.h"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>


class SceneManager {
public:
  SceneManager() = default;
  ~SceneManager() {
    m_scenes.clear();
    m_currentScene = nullptr;
  }

  void RegisterScene(const std::string &name, std::unique_ptr<BaseScene> scene);
  void Update(float deltaTime);
  void Render(SDL_Renderer *render);
  bool ChangeScene(const std::string_view name);
  std::string_view GetCurrentSceneName() const;
  void HandleInput(const SDL_Event &event);
  const std::vector<std::string_view> GetAllSceneNames();

private:
  BaseScene *m_currentScene = nullptr;
  std::unordered_map<std::string_view, std::unique_ptr<BaseScene>> m_scenes;
};