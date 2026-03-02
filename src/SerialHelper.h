#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <serial/serial.h>
#include <string>
#include <thread>
#include <vector>

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
  bool isConnected() const { return _connected; }

  /**
   * @brief 获取并清空接收到的所有数据行（非阻塞，线程安全）
   * @return 包含所有新行字符串的列表
   */
  std::vector<std::string> fetchNewData();

private:
  void threadWorker(); // 后台线程函数
  bool tryConnect();
  void closeConnection();
  bool isMySTM32(const serial::PortInfo &info);

private:
  // 配置
  const uint32_t _baudrate;
  const std::string _targetVid;
  const std::string _targetPid;

  // 状态
  std::unique_ptr<serial::Serial> _serial;
  std::atomic<bool> _keepRunning{false};
  std::atomic<bool> _connected{false};

  // 线程安全缓冲区
  std::thread _workerThread;
  std::mutex _dataMutex;
  std::vector<std::string> _dataBuffer;
};

