#ifndef IPTABLESFIREWALL_H
#define IPTABLESFIREWALL_H

#include "../enum.h"
#include "../logger.h"
#include "../utility.h"
#include "ifirewall.h"
#include <mutex>
#include <stdexcept>

class IptablesFirewall : public IFirewall {

private:
  std::mutex mtx;
  Logger &primaryLog;
  bool sudo;

  IptablesFirewall(Logger &log, bool useSudo);
  bool validate(const std::string &ipAddr, uint16_t port);

public:
  ~IptablesFirewall();
  bool allow_in(const std::string &ip, const Protocol protocol,
                const uint16_t port) override;
  bool removeRule(const std::string &ip, const Protocol protocol,
                  const uint16_t port) override;
  bool block(const std::string &ip) override;
  bool unblock(const std::string &ip) override;

  static IptablesFirewall &getInstance(Logger &log, bool sudo);

  IptablesFirewall(const IptablesFirewall &cpy) = delete;
  IptablesFirewall &operator=(const IptablesFirewall &cpy) = delete;
  IptablesFirewall(IptablesFirewall &org) = delete;
  IptablesFirewall &operator=(IptablesFirewall &&org) = delete;
};

#endif