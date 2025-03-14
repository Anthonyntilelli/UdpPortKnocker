#include "mockFirewall.h"

MockFirewall::MockFirewall(Logger &log) : primaryLog{log}, mtx{}, ruleCount{} {
  primaryLog.log("Server Starting: MockFirewall class is created");
  primaryLog.log(
      "Warning: MockFirewall only posts to logs, no real ports are open");
}

MockFirewall::~MockFirewall() {
  try {
    primaryLog.log("Server Stopping: MockFirewall class is destroyed");
  } catch (...) {
    std::cerr << "Error in closing the MockFirewall Class." << std::endl;
  }
  if (ruleCount != 0)
    primaryLog.log("Error: Mockfirewall has " + std::to_string(ruleCount) +
                   " leftover rules.");
}

bool MockFirewall::allow_in(const std::string &ip, Protocol protocol,
                            uint16_t port) {
  std::lock_guard<std::mutex> lck(mtx);
  std::string strProtocol = (protocol == Protocol::tcp) ? "tcp" : "udp";

  primaryLog.log("Succeeded in Adding for Firewall rule Allow IP: " + ip +
                 " Protocol: " + strProtocol +
                 " Port: " + std::to_string(port));
  ruleCount++;
  return true;
}

bool MockFirewall::removeRule(const std::string &ip, Protocol protocol,
                              uint16_t port) {
  std::lock_guard<std::mutex> lck(mtx);
  std::string strProtocol = (protocol == Protocol::tcp) ? "tcp" : "udp";

  primaryLog.log("Rule REMOVED for Allow IP:" + ip + " Protocol: " +
                 strProtocol + " Port: " + std::to_string(port));
  ruleCount--;
  return true;
}

bool MockFirewall::block(const std::string &ip) {
  std::lock_guard<std::mutex> lck(mtx);
  primaryLog.log("Ban Rule added for IP:" + ip);
  ruleCount++;
  return true;
}

bool MockFirewall::unblock(const std::string &ip) {
  std::lock_guard<std::mutex> lck(mtx);
  primaryLog.log("Ban Rule removed for IP:" + ip);
  ruleCount--;
  return true;
}

MockFirewall &MockFirewall::getInstance(Logger &log) {
  static MockFirewall instance(log);
  return instance;
}
