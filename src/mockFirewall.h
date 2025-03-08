#ifndef MOCKFIREWALL_H
#define MOCKFIREWALL_H

#include "ifirewall.h"
#include "enum.h"
#include <list>
#include <utility>
#include <mutex>
#include <fstream>
#include <unordered_map>

class MockFirewall : public IFirewall {

    private:
      /// IP Address, {port, isTcp}
      std::unordered_map<std::string,std::unordered_map<int, bool>> allowRules;
      // Ip address, Time
      std::list<std::pair<std::string, int>> banList;
      std::mutex mtx;
      std::ofstream transactionLog;
      const std::string logPath{"./mockFirewall.log"};
      Logger &primaryLog;

      MockFirewall(Logger &log);
      //Return success
      bool addRule(std::string ip, Protocol protocol, size_t port) noexcept;
      //Return success
      bool removeRule(std::string ip, Protocol protocol, size_t port) noexcept;
    public:
      ~MockFirewall();
      bool allow_in(std::string ip, Protocol protocol, size_t port);
      bool block(std::string ip, size_t timeSec);
      bool unblock(std::string ip, size_t timeSec);

      static MockFirewall& getInstance(Logger &Log);

      MockFirewall(const MockFirewall &cpy) = delete;
      MockFirewall &operator=(const MockFirewall &cpy) = delete;
      MockFirewall (MockFirewall &org) = delete;
      MockFirewall &operator=(MockFirewall &&org) = delete;
};
#endif