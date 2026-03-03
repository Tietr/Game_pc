#include "SerialHelper.h"

STM32SerialManager::STM32SerialManager(std::string vid, std::string pid,
                                       uint32_t baudrate)
    : m_baudrate(baudrate), m_targetVid(vid), m_targetPid(pid) {}

STM32SerialManager::~STM32SerialManager() { stop(); }

void STM32SerialManager::start() {
  if (m_keepRunning)
    return;
  m_keepRunning = true;
  m_workerThread = std::thread(&STM32SerialManager::threadWorker, this);
}

void STM32SerialManager::stop() {
  m_keepRunning = false;
  if (m_workerThread.joinable()) {
    m_workerThread.join();
  }
  closeConnection();
}

void STM32SerialManager::threadWorker() {
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

// std::string STM32SerialManager::fetchNewData() {
//   std::lock_guard<std::mutex> lock(m_dataMutex);
//   std::vector<std::string> temp;
//   temp.swap(m_dataBuffer);
//   return temp.at(0);
// }

std::optional<std::string> STM32SerialManager::fetchNewData() {
  std::lock_guard<std::mutex> lock(m_dataMutex);

  if (m_dataBuffer.empty()) {
    return std::nullopt; // 或者返回空字符串 ""，但 optional 语义更清晰
  }
  std::string data = std::move(m_dataBuffer.front());
  m_dataBuffer.pop_front(); // deque 的 pop_front 非常快
  return data;
}


bool STM32SerialManager::tryConnect() {
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

void STM32SerialManager::closeConnection() {
  if (m_serial) {
    if (m_serial->isOpen())
      m_serial->close();
    m_serial.reset();
  }
}

bool STM32SerialManager::isMySTM32(const serial::PortInfo &info) {
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