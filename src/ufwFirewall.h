#ifndef UFWFIREWALL_H
#define UFWFIREWALL_H

// https://www.cyberciti.biz/faq/howto-adding-comments-to-ufw-firewall-rule/
// sudo ufw allow 443 comment 'Allow HTTPS (Nginx) access'
// ufw delete allow 22/tcp

#include "ifirewall.h"
#include "logger.h"
#include "enum.h"
#include "utility.h"
#include <mutex>
#include <stdexcept>

class UfwFirewall : public IFirewall {

    private:
      std::mutex mtx;
      Logger &primaryLog;
      bool sudo;

      UfwFirewall(Logger &log, bool useSudo);
      bool validateIp(std::string ipaAddr);
      bool validatePort(size_t port);
    public:
      ~UfwFirewall();
      bool allow_in(std::string ip, Protocol protocol, size_t port);
      bool removeRule(std::string ip, Protocol protocol, size_t port);
      bool block(std::string ip);
      bool unblock(std::string ip);

      static UfwFirewall& getInstance(Logger &log, bool sudo);

      UfwFirewall(const UfwFirewall &cpy) = delete;
      UfwFirewall &operator=(const UfwFirewall &cpy) = delete;
      UfwFirewall (UfwFirewall &org) = delete;
      UfwFirewall &operator=(UfwFirewall &&org) = delete;
};
#endif