#ifndef FIREWALL_H
#define FIREWALL_H

#include <stdexcept>
#include <string>
#include "../enum.h"
#include "../logger.h"

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
  static IFirewall& getInstance(const firewallType type, const Logger &log){
    switch(type){
        case (firewallType::invalid):
          throw std::runtime_error{"Error:This is not a valid type."};
        break;
        case (firewallType::mock):
          throw std::runtime_error{"Error: Not Implemented"};
        break;
        case (firewallType::ufw):
          throw std::runtime_error{"Error: Not Implemented"};        
        break;
        case (firewallType::firewalld):
          throw std::runtime_error{"Error: Not Implemented"};
        break;
        case (firewallType::iptables):
          throw std::runtime_error{"Error: Not Implemented"};
        break;
        default:
          throw std::runtime_error{"Error: Should not get here."};
        break;
    }
  }
};
#endif