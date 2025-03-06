#ifndef MOCKFIREWALL_H
#define MOCKFIREWALL_H

#include "ifirewall.h"
#include <list>
#include <utility>
#include <mutex>
#include <fstream>

class MockFirewall final: IFirewall {
    private:
      /// IP Address, port
      std::list<std::pair<std::string, int>> AllowRules;
      // Ip address, Time
      std::list<std::pair<std::string, int>> banList;
      std::mutex mtx;
      std::ofstream transactionLog;
      const std::string logPath{"./MockFirewall.log"};
      Logger &primaryLog;

      MockFirewall(Logger &log);
      //Return success
      bool addRule(std::string ip, Protocol protocol, size_t port) noexcept;
      //Return success
      bool removeRule(std::string ip, Protocol protocol, size_t port) noexcept;
    public:
      ~MockFirewall() override;
      bool allow_in(std::string ip, Protocol protocol, size_t port) override;
      bool block(std::string ip, size_t timeSec) override;

      MockFirewall(const MockFirewall &cpy) = delete;
      MockFirewall &operator=(const MockFirewall &cpy) = delete;
};


#endif