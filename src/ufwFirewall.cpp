#include "ufwFirewall.h"

UfwFirewall::UfwFirewall(Logger &log):primaryLog{log},mtx{} {}

UfwFirewall::~UfwFirewall() {}

bool UfwFirewall::allow_in(std::string ip, Protocol protocol, size_t port) {
  return false;
}

bool UfwFirewall::removeRule(std::string ip, Protocol protocol, size_t port) {
  return false;
}

bool UfwFirewall::block(std::string ip) { return false; }
bool UfwFirewall::unblock(std::string ip) { return false; }

UfwFirewall& UfwFirewall::getInstance(Logger &log){
    static UfwFirewall instance(log);
    return instance;
}
