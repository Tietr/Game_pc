#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <serial/serial.h>
#include <string>
#include <thread>

class SerialManager {
public:
  SerialManager(std::string vid = "0483", std::string pid = "5740",
                uint32_t baudrate = 115200);
  ~SerialManager();
  SerialManager(const SerialManager &) = delete;
  SerialManager &operator=(const SerialManager &) = delete;

  void start();

  void stop();

  inline bool isConnected() const { return m_connected; }

  std::optional<std::string> fetchNewData();

private:
  void threadWorker(); 
  bool tryConnect();
  void closeConnection();
  bool isMySTM32(const serial::PortInfo &info);

private:

  const uint32_t m_baudrate;
  const std::string m_targetVid;
  const std::string m_targetPid;
  std::unique_ptr<serial::Serial> m_serial;
  std::atomic<bool> m_keepRunning{false};
  std::atomic<bool> m_connected{false};
  std::thread m_workerThread;
  std::mutex m_dataMutex;
  std::deque<std::string> m_dataBuffer;
};
