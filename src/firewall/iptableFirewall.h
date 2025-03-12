#ifndef IPTABLESFIREWALL_H
#define IPTABLESFIREWALL_H

#include "../enum.h"
#include "ifirewall.h"
#include "../logger.h"
#include "../utility.h"
#include <mutex>
#include <stdexcept>

class IptablesFirewall : public IFirewall {

private:
  std::mutex mtx;
  Logger &primaryLog;
  bool sudo;

  IptablesFirewall(Logger &log, bool useSudo);
  bool validate(std::string &ipAddr, size_t port);

public:
  ~IptablesFirewall();
  bool allow_in(std::string &ip, Protocol protocol, size_t port);
  bool removeRule(std::string &ip, Protocol protocol, size_t port);
  bool block(std::string &ip);
  bool unblock(std::string &ip);

  static IptablesFirewall &getInstance(Logger &log, bool sudo);

  IptablesFirewall(const IptablesFirewall &cpy) = delete;
  IptablesFirewall &operator=(const IptablesFirewall &cpy) = delete;
  IptablesFirewall(IptablesFirewall &org) = delete;
  IptablesFirewall &operator=(IptablesFirewall &&org) = delete;
};

#endif