#pragma once
#include "BaseScene.h"
#include <memory>
#include <string>
#include <unordered_map>

class SceneManager {
public:
  SceneManager() = default;
  ~SceneManager() = default;
  SceneManager(const SceneManager &) = delete;
  SceneManager &operator=(const SceneManager &) = delete;
  SceneManager(SceneManager &&) = default;
  SceneManager &operator=(SceneManager &&) = default;

  void RegisterScene(const std::string &name, std::shared_ptr<BaseScene> scene);
  void Update(float deltaTime);
  void Render(SDL_Renderer *render, TTF_Font *font);
  bool ChangeScene(const std::string &name);
  std::string GetCurrentSceneName() const;
  void HandleInput(const SDL_Event &event);
  std::vector<std::string> GetAllSceneNames();
  void AddFlashUIToCurrentScene(std::unique_ptr<BaseUIItem> item);
  void RemoveFlashUIFromCurrentScene(size_t index);
  int GetCurrentFlashingUIIndex() const;
  inline std::shared_ptr<BaseScene> GetCurrentScene() const {
    return m_currentScene;
  }
  void EnableFlashForCurrentScene() {
    if (m_currentScene) {
      m_currentScene->EnableFlash();
    }
  }
  void ResetFlashStateForCurrentScene() {
    if (m_currentScene) {
      m_currentScene->DisableFlash();
    }
  }

  bool IsCurrentSceneFlashing() const {
    if (m_currentScene) {
      return m_currentScene->IsFlashing();
    }
    return false;
  }


private:
  std::shared_ptr<BaseScene> m_currentScene;
  std::unordered_map<std::string, std::shared_ptr<BaseScene>> m_scenes;
};

// 这个scenemanager应该管理所有场景的切换和更新（渲染模式转换）等