#include "SceneManager.h"
#include "BaseApp.h"

void SceneManager::RegisterScene(const std::string &name,
                                 std::shared_ptr<BaseScene> scene) {
  if (m_scenes.find(name) != m_scenes.end()) {
    std::cout << "Scene " << name << " already exists" << std::endl;
    return;
  }
  scene->OnInit(name);
  m_scenes[name] = scene;
}
void SceneManager::Update(float deltaTime) {
  auto currentScene = m_currentScene.lock();
  if (!currentScene) {
    return;
  }
  currentScene->Update(deltaTime);
  if (currentScene->GetNextSceneName() == "quit") {
    BaseApp::QuitGame();
  }
  if (currentScene->GetName() != currentScene->GetNextSceneName()) {
    ChangeScene(currentScene->GetNextSceneName());
  }
}
void SceneManager::Render(SDL_Renderer *render, TTF_Font *font) {
  auto currentScene = m_currentScene.lock();
  if (!currentScene) {
    return;
  }
  currentScene->Render(render,font);
}
bool SceneManager::ChangeScene(const std::string &name) {
  auto newScene = m_scenes.find(name);
  if (newScene == m_scenes.end()) {
    std::cout << "Scene " << name << " not found" << std::endl;
    return false;
  }
  std::shared_ptr<BaseScene> currentScene = m_currentScene.lock();
  if (currentScene && name == currentScene->GetName()) {
    return true;
  }
  if (currentScene) {
    currentScene->OnExit();
  }
  m_currentScene = newScene->second;
  newScene->second->OnEnter();
  return true;
}
std::string SceneManager::GetCurrentSceneName() const {
  auto currentScene = m_currentScene.lock();
  if (!currentScene) {
    return "";
  }
  return currentScene->GetName();
}
void SceneManager::HandleInput(const SDL_Event &event) {
  auto currentScene = m_currentScene.lock();
  if (!currentScene) {
    return;
  }
  currentScene->HandleInput(event);
}
std::vector<std::string> SceneManager::GetAllSceneNames() {
  std::vector<std::string> names;
  for (auto &scene : m_scenes) {
    names.push_back(scene.first);
  }
  return names;
}
