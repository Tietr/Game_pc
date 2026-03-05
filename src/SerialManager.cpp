#include "SerialManager.h"

SerialManager::SerialManager(std::string vid, std::string pid,
                             uint32_t baudrate)
    : m_baudrate(baudrate), m_targetVid(vid), m_targetPid(pid) {}

SerialManager::~SerialManager() { stop(); }

void SerialManager::start() {
  if (m_keepRunning)
    return;
  m_keepRunning = true;
  m_workerThread = std::thread(&SerialManager::threadWorker, this);
}

void SerialManager::stop() {
  m_keepRunning = false;
  if (m_workerThread.joinable()) {
    m_workerThread.join();
  }
  closeConnection();
}

void SerialManager::threadWorker() {
  while (m_keepRunning) {
    if (!m_serial || !m_serial->isOpen()) {
      m_connected = false;
      if (!tryConnect()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }
      m_connected = true;
    }

    try {
      if (m_serial->available()) {
        std::string line = m_serial->readline();
        if (!line.empty()) {
          // 将数据存入缓冲区
          std::lock_guard<std::mutex> lock(m_dataMutex);
          m_dataBuffer.push_back(line);
        }
      }
    } catch (const std::exception &e) {
      m_connected = false;
      closeConnection();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
}

std::optional<std::string> SerialManager::fetchNewData() {
  std::lock_guard<std::mutex> lock(m_dataMutex);

  if (m_dataBuffer.empty()) {
    return std::nullopt;
  }
  std::string data = std::move(m_dataBuffer.front());
  m_dataBuffer.pop_front();
  return data;
}

bool SerialManager::tryConnect() {
  auto devices = serial::list_ports();
  for (const auto &device : devices) {
    if (isMySTM32(device)) {
      try {
        m_serial = std::make_unique<serial::Serial>(
            device.port, m_baudrate, serial::Timeout::simpleTimeout(1000));
        return m_serial->isOpen();
      } catch (...) {
        return false;
      }
    }
  }
  return false;
}

void SerialManager::closeConnection() {
  if (m_serial) {
    if (m_serial->isOpen())
      m_serial->close();
    m_serial.reset();
  }
}

bool SerialManager::isMySTM32(const serial::PortInfo &info) {
  return (info.hardware_id.find("VID_" + m_targetVid) != std::string::npos) &&
         (info.hardware_id.find("PID_" + m_targetPid) != std::string::npos);
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