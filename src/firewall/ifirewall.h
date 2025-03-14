#ifndef FIREWALL_H
#define FIREWALL_H

#include "../enum.h"
#include "../logger.h"
#include <stdexcept>
#include <string>

struct FirewallRule {
  std::string ip;
  Protocol protocol;
  uint16_t port;
};

// SINGLETONS instance
class IFirewall {
protected:
  IFirewall() = default;

public:
  virtual ~IFirewall() = default;
  virtual bool allow_in(const std::string &ip, Protocol protocol,
                        uint16_t port) = 0;
  virtual bool removeRule(const std::string &ip, Protocol protocol,
                          uint16_t port) = 0;
  virtual bool block(const std::string &ip) = 0;
  virtual bool unblock(const std::string &ip) = 0;

  IFirewall(const IFirewall &cpy) = delete;
  IFirewall &operator=(const IFirewall &cpy) = delete;
  IFirewall(IFirewall &org) = delete;
  IFirewall &operator=(IFirewall &&org) = delete;
};
#endif
