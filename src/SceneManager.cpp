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
  if (!m_currentScene) {
    return;
  }

  m_currentScene->Update(deltaTime);
  const std::string nextSceneName = m_currentScene->GetNextSceneName();
  if (nextSceneName == "quit") {
    BaseApp::QuitGame();
    return;
  }
  if (m_currentScene->GetName() != nextSceneName) {
    ChangeScene(nextSceneName);
  }
}
void SceneManager::Render(SDL_Renderer *render, TTF_Font *font) {
  if (!m_currentScene) {
    return;
  }
  m_currentScene->Render(render, font);
}
bool SceneManager::ChangeScene(const std::string &name) {
  auto newScene = m_scenes.find(name);
  if (newScene == m_scenes.end()) {
    std::cout << "Scene " << name << " not found" << std::endl;
    return false;
  }
  std::shared_ptr<BaseScene> newScenePtr = newScene->second;
  if (m_currentScene && name == m_currentScene->GetName()) {
    return true;
  }
  if (!newScenePtr) {
    std::cout << "Scene " << name << " is null" << std::endl;
    return false;
  }
  if (m_currentScene) {
    m_currentScene->OnExit();
  }
  m_currentScene = newScenePtr;
  newScenePtr->OnEnter();

  return true;
}
std::string SceneManager::GetCurrentSceneName() const {

  if (!m_currentScene) {
    return "";
  }
  return m_currentScene->GetName();
}
void SceneManager::HandleInput(const SDL_Event &event) {

  if (!m_currentScene) {
    return;
  }
  m_currentScene->HandleInput(event);
}
// 获取所有场景名称
std::vector<std::string> SceneManager::GetAllSceneNames() {
  std::vector<std::string> names;
  for (auto &scene : m_scenes) {
    names.push_back(scene.first);
  }
  return names;
}
void SceneManager::AddFlashUIToCurrentScene(std::unique_ptr<BaseUIItem> item) {
  if (m_currentScene) {
    m_currentScene->AddFlashUIDelayed(std::move(item));
  }
}
void SceneManager::RemoveFlashUIFromCurrentScene(size_t index) {
  if (m_currentScene) {
    m_currentScene->RemoveFlashUIDelayed(index);
  }
}
int SceneManager::GetCurrentFlashingUIIndex() const {
  if (!m_currentScene) {
    return -1;
  }
  return m_currentScene->GetCurrentSceneFlashUIIndex();
}
