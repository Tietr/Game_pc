// #include <functional>
// #include <map>
// #include <string>
// #include <vector>


// // 事件总线
// class EventBus {
// public:
//   using Callback = std::function<void(const std::string &)>;

//   // 单例模式，方便跨层级访问
//   static EventBus &getInstance() {
//     static EventBus instance;
//     return instance;
//   }

//   // 订阅事件 (上层调用)
//   void subscribe(const std::string &eventName, Callback cb) {
//     listeners[eventName].push_back(cb);
//   }

//   // 发布事件 (底层触发)
//   void publish(const std::string &eventName, const std::string &data) {
//     if (listeners.find(eventName) != listeners.end()) {
//       for (auto &cb : listeners[eventName]) {
//         cb(data);
//       }
//     }
//   }

// private:
//   std::map<std::string, std::vector<Callback>> listeners;
// };