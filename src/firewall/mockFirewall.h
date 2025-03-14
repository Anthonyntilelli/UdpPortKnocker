#ifndef MOCKFIREWALL_H
#define MOCKFIREWALL_H

#include "../enum.h"
#include "../utility.h"
#include "ifirewall.h"
#include <fstream>
#include <list>
#include <mutex>
#include <unordered_map>
#include <utility>

class MockFirewall : public IFirewall {

private:
  Logger &primaryLog;
  std::mutex mtx;
  unsigned int ruleCount;

  MockFirewall(Logger &log);

public:
  ~MockFirewall();
  bool allow_in(const std::string &ip, Protocol protocol,
                uint16_t port) override;
  bool removeRule(const std::string &ip, Protocol protocol,
                  uint16_t port) override;
  bool block(const std::string &ip) override;
  bool unblock(const std::string &ip) override;

  static MockFirewall &getInstance(Logger &Log);

  MockFirewall(const MockFirewall &cpy) = delete;
  MockFirewall &operator=(const MockFirewall &cpy) = delete;
  MockFirewall(MockFirewall &org) = delete;
  MockFirewall &operator=(MockFirewall &&org) = delete;
};
#endif