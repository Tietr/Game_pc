#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <serial/serial.h>
#include <string>
#include <thread>

class STM32SerialManager {
public:
  STM32SerialManager(std::string vid = "0483", std::string pid = "5740",
                     uint32_t baudrate = 115200);
  ~STM32SerialManager();

  // 防止拷贝
  STM32SerialManager(const STM32SerialManager &) = delete;
  STM32SerialManager &operator=(const STM32SerialManager &) = delete;

  // --- 游戏逻辑接口 ---

  /**
   * @brief 启动后台监听线程
   */
  void start();

  /**
   * @brief 停止后台线程
   */
  void stop();

  /**
   * @brief 检查当前是否已连接
   */
  inline bool isConnected() const { return m_connected; }

  /**
   * @brief 获取并清空接收到的所有数据行（非阻塞，线程安全）
   * @return 包含所有新行字符串
   */
  std::optional<std::string> fetchNewData();

private:
  void threadWorker(); // 后台线程函数
  bool tryConnect();
  void closeConnection();
  bool isMySTM32(const serial::PortInfo &info);

private:
  // 配置
  const uint32_t m_baudrate;
  const std::string m_targetVid;
  const std::string m_targetPid;

  // 状态
  std::unique_ptr<serial::Serial> m_serial;
  std::atomic<bool> m_keepRunning{false};
  std::atomic<bool> m_connected{false};

  // 线程安全缓冲区
  std::thread m_workerThread;
  std::mutex m_dataMutex;
  std::deque<std::string> m_dataBuffer;
};
