#include "mockFirewall.h"

MockFirewall::MockFirewall(Logger &log):primaryLog{log},allowRules{},banList{},mtx{}
{
    transactionLog.open(logPath, std::ios::app);
    if (!transactionLog.is_open()) throw std::runtime_error("Cannot open the Transaction log file at: " + logPath + ".");
    primaryLog.log("Server Starting: MockFirewall class is created");
}

bool MockFirewall::addRule(std::string ip, Protocol protocol, size_t port) noexcept {
    return false;
}

bool MockFirewall::removeRule(std::string ip, Protocol protocol, size_t port) noexcept {
  return false;
}

MockFirewall::~MockFirewall() {
  try {
    primaryLog.log("Server Stopping: MockFirewall class is destroyed");
  } catch(...){
    std::cerr << "Error in closing the Logger Class." << std::endl;
  }

  //TODO: Purge Firewall rule (regular and ban)

  if (transactionLog.is_open()) transactionLog.close();
}


bool MockFirewall::allow_in(std::string ip, Protocol protocol, size_t port) {
  return false;
}

bool MockFirewall::block(std::string ip, size_t timeSec) { return false; }
bool MockFirewall::unblock(std::string ip, size_t timeSec) { return false; }

MockFirewall &MockFirewall::getInstance(Logger &log) {
  static MockFirewall instance(log);
  return instance;
}
