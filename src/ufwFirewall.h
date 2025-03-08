#ifndef UFWFIREWALL_H
#define UFWFIREWALL_H

// https://www.cyberciti.biz/faq/howto-adding-comments-to-ufw-firewall-rule/
// sudo ufw allow 443 comment 'Allow HTTPS (Nginx) access'
// ufw delete allow 22/tcp

#include "ifirewall.h"
#include "logger.h"
#include "enum.h"
#include <mutex>


class UfwFirewall : public IFirewall {

    private:
      std::mutex mtx;
      Logger &primaryLog;

      UfwFirewall(Logger &log);
    public:
      ~UfwFirewall();
      bool allow_in(std::string ip, Protocol protocol, size_t port);
      bool removeRule(std::string ip, Protocol protocol, size_t port);
      bool block(std::string ip);
      bool unblock(std::string ip);

      static UfwFirewall& getInstance(Logger &log);

      UfwFirewall(const UfwFirewall &cpy) = delete;
      UfwFirewall &operator=(const UfwFirewall &cpy) = delete;
      UfwFirewall (UfwFirewall &org) = delete;
      UfwFirewall &operator=(UfwFirewall &&org) = delete;
};
#endif