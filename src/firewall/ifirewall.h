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
  virtual bool allow_in(std::string &ip, Protocol protocol, size_t port) = 0;
  virtual bool removeRule(std::string &ip, Protocol protocol, size_t port) = 0;
  virtual bool block(std::string &ip) = 0;
  virtual bool unblock(std::string &ip) = 0;

  IFirewall(const IFirewall &cpy) = delete;
  IFirewall &operator=(const IFirewall &cpy) = delete;
  IFirewall(IFirewall &org) = delete;
  IFirewall &operator=(IFirewall &&org) = delete;
};
#endif
