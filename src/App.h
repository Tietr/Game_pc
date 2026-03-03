#pragma once
#include "Background.h"
#include "BaseApp.h"
#include "Duck.h"
#include "UI.h"
#include "SerialHelper.h"
#include <array>



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
  STM32SerialManager m_serialManager;
  Background m_background;
  std::vector<Duck> m_ducks;
  UI m_ui;
  // 存储 USB 口接收的数据
  struct Data {
    bool IsDown = false;  // true 表示 Down，false 表示 Up
    uint32_t Value = 0;
    uint32_t Percent = 0;
  };
  // 这个属于游戏的数据，如果满了，则全部丢弃
  std::array<std::optional<Data>, 100> m_dataBuffer;
  
  void analyzeUSBData() {
    auto TempData = m_serialManager.fetchNewData();
    if (TempData) {
      const std::string data = TempData.value();
      
      //data :Down, Value: 428, Percent:89%
      Data newData;
      if (data.find("Down") != std::string::npos) {
        newData.IsDown = true;
      } else if (data.find("Up") != std::string::npos) {
        newData.IsDown = false;
      }
      size_t valuePos = data.find("Value:");
      if (valuePos != std::string::npos) {
        size_t valueStart = valuePos + 6;
        size_t valueEnd = data.find(",", valueStart);
        if (valueEnd != std::string::npos) {
          std::string valueStr = data.substr(valueStart, valueEnd - valueStart);
          valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
          newData.Value = std::stoi(valueStr);
        }
      }
      size_t percentPos = data.find("Percent:");
      if (percentPos != std::string::npos) {
        size_t percentStart = percentPos + 8;
        size_t percentEnd = data.find("%", percentStart);
        if (percentEnd != std::string::npos) {
          std::string percentStr = data.substr(percentStart, percentEnd - percentStart);
          percentStr.erase(std::remove_if(percentStr.begin(), percentStr.end(), ::isspace), percentStr.end());
          newData.Percent = std::stoi(percentStr);
        }
      }
      // 存入缓冲区 - 使用 std::optional 的 has_value() 判断是否为空
      int index = 0;
      while (index < m_dataBuffer.size() && m_dataBuffer[index].has_value()) {
        index++;
      }
      if (index < m_dataBuffer.size()) {
        m_dataBuffer[index] = newData;  // std::optional 自动包装
      }
    }
  }
};