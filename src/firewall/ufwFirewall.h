#ifndef UFWFIREWALL_H
#define UFWFIREWALL_H

#include "../enum.h"
#include "../logger.h"
#include "../utility.h"
#include "ifirewall.h"
#include <mutex>
#include <stdexcept>

class UfwFirewall : public IFirewall {

private:
  std::mutex mtx;
  Logger &primaryLog;
  bool sudo;

  UfwFirewall(Logger &log, bool useSudo);
  bool validate(const std::string &ipAddr, const uint16_t port);

public:
  ~UfwFirewall();
  bool allow_in(const std::string &ip, const Protocol protocol,
                const uint16_t port) override;
  bool removeRule(const std::string &ip, const Protocol protocol,
                  const uint16_t port) override;
  bool block(const std::string &ip) override;
  bool unblock(const std::string &ip) override;

  static UfwFirewall &getInstance(Logger &log, bool sudo);

  UfwFirewall(const UfwFirewall &cpy) = delete;
  UfwFirewall &operator=(const UfwFirewall &cpy) = delete;
  UfwFirewall(UfwFirewall &org) = delete;
  UfwFirewall &operator=(UfwFirewall &&org) = delete;
};
#endif