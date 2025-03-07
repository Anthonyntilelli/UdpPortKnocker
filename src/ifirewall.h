#ifndef FIREWALL_H
#define FIREWALL_H

#include <stdexcept>
#include <string>
#include "enum.h"
#include "logger.h"

// SINGLETONS instance
class IFirewall {
protected:
  IFirewall() = default;
public:
   virtual ~IFirewall()=default;
   virtual bool allow_in(std::string ip, Protocol protocol, size_t port)=0;
   virtual bool block(std::string ip, size_t timeSec)=0;

  IFirewall(const IFirewall &cpy) = delete;
  IFirewall &operator=(const IFirewall &cpy) = delete;
};
#endif
