#include "mockFirewall.h"

MockFirewall::MockFirewall(Logger &log):primaryLog{log},mtx{}
{
    primaryLog.log("Server Starting: MockFirewall class is created");
    primaryLog.log("Warning: MockFirewall only posts to logs, no real ports are open");
}

MockFirewall::~MockFirewall() {
  try {
    primaryLog.log("Server Stopping: MockFirewall class is destroyed");
  } catch(...){
    std::cerr << "Error in closing the Logger Class." << std::endl;
  }
}

bool MockFirewall::allow_in(std::string ip, Protocol protocol, size_t port) {
  std::lock_guard<std::mutex> lck(mtx);
  std::string strProtocol = (protocol == Protocol::tcp) ? "tcp" : "udp";

  primaryLog.log("Succeeded in Adding for  Allow IP:" + ip + "Protocol: " + strProtocol + "Port: " + std::to_string(port));
  return true;
}

bool MockFirewall::removeRule(std::string ip, Protocol protocol, size_t port) {
  std::lock_guard<std::mutex> lck(mtx);
  std::string strProtocol = (protocol == Protocol::tcp) ? "tcp" : "udp";

  primaryLog.log("Rule REMOVED for Allow IP:" + ip + " Protocol: " + strProtocol + " Port: " + std::to_string(port));
  return true;
}

bool MockFirewall::block(std::string ip) {
  std::lock_guard<std::mutex> lck(mtx);
  primaryLog.log("Ban Rule added for IP:" + ip);
  return true;
}

bool MockFirewall::unblock(std::string ip) {
  std::lock_guard<std::mutex> lck(mtx);
  primaryLog.log("Ban Rule removed for IP:" + ip);
  return true;
}

MockFirewall &MockFirewall::getInstance(Logger &log) {
  static MockFirewall instance(log);
  return instance;
}
