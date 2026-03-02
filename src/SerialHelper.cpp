#include "SerialHelper.h"
#include <chrono>



STM32SerialManager::STM32SerialManager(std::string vid, std::string pid,
                                       uint32_t baudrate)
    : _baudrate(baudrate), _targetVid(vid), _targetPid(pid) {}

STM32SerialManager::~STM32SerialManager() { stop(); }

void STM32SerialManager::start() {
  if (_keepRunning)
    return;
  _keepRunning = true;
  _workerThread = std::thread(&STM32SerialManager::threadWorker, this);
}

void STM32SerialManager::stop() {
  _keepRunning = false;
  if (_workerThread.joinable()) {
    _workerThread.join();
  }
  closeConnection();
}

void STM32SerialManager::threadWorker() {
  while (_keepRunning) {
    if (!_serial || !_serial->isOpen()) {
      _connected = false;
      if (!tryConnect()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }
      _connected = true;
    }

    try {
      if (_serial->available()) {
        std::string line = _serial->readline();
        if (!line.empty()) {
          // 将数据存入缓冲区
          std::lock_guard<std::mutex> lock(_dataMutex);
          _dataBuffer.push_back(line);
        }
      }
    } catch (const std::exception &e) {
      _connected = false;
      closeConnection();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
}

std::vector<std::string> STM32SerialManager::fetchNewData() {
  std::lock_guard<std::mutex> lock(_dataMutex);
  std::vector<std::string> temp;
  temp.swap(_dataBuffer); // 快速交换，清空原缓冲区并返回数据
  return temp;
}

bool STM32SerialManager::tryConnect() {
  auto devices = serial::list_ports();
  for (const auto &device : devices) {
    if (isMySTM32(device)) {
      try {
        _serial = std::make_unique<serial::Serial>(
            device.port, _baudrate, serial::Timeout::simpleTimeout(1000));
        return _serial->isOpen();
      } catch (...) {
        return false;
      }
    }
  }
  return false;
}

void STM32SerialManager::closeConnection() {
  if (_serial) {
    if (_serial->isOpen())
      _serial->close();
    _serial.reset();
  }
}

bool STM32SerialManager::isMySTM32(const serial::PortInfo &info) {
  return (info.hardware_id.find("VID_" + _targetVid) != std::string::npos) &&
         (info.hardware_id.find("PID_" + _targetPid) != std::string::npos);
}

/*

#include "STM32SerialManager.h"

class MyGame {
    STM32SerialManager serial;
    bool running = true;

public:
    void Init() {
        // 游戏启动时开启串口后台线程
        serial.start();
    }

    void Update() {
        // 每一帧轮询数据（非阻塞）
        auto inputs = serial.fetchNewData();
        for (const auto& msg : inputs) {
            // 解析数据，例如：根据 STM32 发来的 "J_UP" 移动角色
            if (msg.find("J_UP") != std::string::npos) {
                player.moveUp();
            }
        }
    }

    void Draw() {
        // 在 UI 上显示连接状态
        if (serial.isConnected()) {
            DrawText("Controller: Connected", 10, 10, 20, GREEN);
        } else {
            DrawText("Controller: Disconnected (Scanning...)", 10, 10, 20, RED);
        }
    }

    void Shutdown() {
        serial.stop(); // 游戏退出时关闭线程
    }
};


*/