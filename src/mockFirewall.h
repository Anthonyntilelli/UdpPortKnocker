#ifndef MOCKFIREWALL_H
#define MOCKFIREWALL_H

#include "ifirewall.h"
#include "enum.h"
#include "utility.h"
#include <list>
#include <utility>
#include <mutex>
#include <fstream>
#include <unordered_map>

class MockFirewall : public IFirewall {

    private:
      std::mutex mtx;
      Logger &primaryLog;

      MockFirewall(Logger &log);
    public:
      ~MockFirewall();
      bool allow_in(std::string ip, Protocol protocol, size_t port);
      bool removeRule(std::string ip, Protocol protocol, size_t port);
      bool block(std::string ip);
      bool unblock(std::string ip);

      static MockFirewall& getInstance(Logger &Log);

      MockFirewall(const MockFirewall &cpy) = delete;
      MockFirewall &operator=(const MockFirewall &cpy) = delete;
      MockFirewall (MockFirewall &org) = delete;
      MockFirewall &operator=(MockFirewall &&org) = delete;
};
#endif