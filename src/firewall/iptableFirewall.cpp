#include "iptableFirewall.h"

IptablesFirewall::IptablesFirewall(Logger &log, bool useSudo)
    : mtx{}, primaryLog{log}, sudo{useSudo} {
  std::lock_guard{mtx};
  auto result = utility::execCommand(
      "iptables --version"); // do not need sudo for this command.
  if (result.exitCode == 127)
    throw std::runtime_error{"Iptables is not installed."};
  if (result.exitCode != 0)
    throw std::runtime_error{"Iptables Error: During version check."};

  size_t frontPos = result.output.find('v');
  size_t endPos = result.output.find('.');
  if (frontPos != std::string::npos && endPos != std::string::npos) {
    auto majorVersion =
        result.output.substr(frontPos + 1, endPos - frontPos - 1);
    if (majorVersion != "1")
      log.log("Warning: This program was was tested on iptables major version "
              "0. This program may not work on newer versions.");
  } else
    log.log("Version check on iptables failed, will attempt anyway.");
  primaryLog.log("Server Starting: Iptable class is created");
}

// Works for ipv4 only
bool IptablesFirewall::validate(const std::string &ipAddr, uint16_t port) {
  // Validate IP
  unsigned char buf[sizeof(struct in6_addr)];
  auto check = inet_pton(AF_INET, ipAddr.c_str(), buf);
  if (check <= 0)
    return false;

  // validate port
  return (port >= 1);
}

IptablesFirewall::~IptablesFirewall() {
  try {
    primaryLog.log("Server Stopping: Iptables Firewall class is destroyed");
  } catch (...) {
    std::cerr << "Error in closing the IptablesFirewall Class." << std::endl;
  }
}

bool IptablesFirewall::allow_in(const std::string &ip, const Protocol protocol,
                                const uint16_t port) {
  std::lock_guard{mtx};
  if (!validate(ip, port)) {
    primaryLog.log(
        "Warning: Invalid port or ip sent to Iptable firewall allow_in rule");
    return false;
  }

  auto proto = (protocol == Protocol::udp) ? "udp" : "tcp";
  auto sPort = std::to_string(port);
  auto command = "iptables -A INPUT -d " + ip + " -p " + proto + " --dport " +
                 sPort + " -m comment --comment " + "\"" + "udpPortKnocker," +
                 ip + "," + proto + "," + sPort +
                 "\""
                 " -j ACCEPT";
  if (sudo)
    command = "sudo " + command;
  auto result = utility::execCommand(command);

  if (result.exitCode != 0) {
    primaryLog.log("Failed to add Rule for  Allow IP:" + ip +
                   " Protocol: " + proto + "Port: " + sPort);
    return false;
  }
  primaryLog.log("Succeeded in Adding for firewall rule Allow IP: " + ip +
                 " Protocol: " + proto + " Port: " + sPort);
  return true;
}

bool IptablesFirewall::removeRule(const std::string &ip,
                                  const Protocol protocol,
                                  const uint16_t port) {
  std::lock_guard{mtx};
  if (!validate(ip, port)) {
    primaryLog.log(
        "Warning: Invalid port or ip sent to Iptable firewall allow_in rule");
    return false;
  }

  auto proto = (protocol == Protocol::udp) ? "udp" : "tcp";
  auto sPort = std::to_string(port);
  auto command = "iptables -D INPUT -d " + ip + " -p " + proto + " --dport " +
                 sPort + " -m comment --comment " + "\"" + "udpPortKnocker," +
                 ip + "," + proto + "," + sPort +
                 "\""
                 " -j ACCEPT";
  if (sudo)
    command = "sudo " + command;
  auto result = utility::execCommand(command);

  if (result.exitCode != 0) {
    primaryLog.log("Failed to remove Rule for  Allow IP:" + ip +
                   " Protocol: " + proto + "Port: " + std::to_string(port));
    return false;
  }

  primaryLog.log("Succeeded in removed for Allow rule IP:" + ip +
                 " Protocol: " + proto + "Port: " + std::to_string(port));
  return true;
}

// sudo iptables -A INPUT -s 192.168.1.100 -j DROP
bool IptablesFirewall::block(const std::string &ip) {
  std::lock_guard{mtx};
  if (!validate(ip, 99)) {
    primaryLog.log("Warning: Invalid IP sent to Iptables firewall ban rule");
    return false;
  }

  auto command = "iptables -A INPUT -s " + ip + " -m comment --comment " +
                 "\"" + "udpPortKnocker," + ip + "\"" + " -j DROP";
  if (sudo)
    command = "sudo " + command;

  auto result = utility::execCommand(command);
  if (result.exitCode != 0) {
    primaryLog.log("Failed to add Rule for Banning IP:" + ip);
    return false;
  }
  primaryLog.log("Succeeded in Banning IP:" + ip);
  return true;
}

bool IptablesFirewall::unblock(const std::string &ip) {
  std::lock_guard{mtx};
  if (!validate(ip, 99)) {
    primaryLog.log("Warning: Invalid ip sent to iptable firewall unban rule");
    return false;
  }

  auto command = "iptables -D INPUT -s " + ip + " -m comment --comment " +
                 "\"" + "udpPortKnocker," + ip + "\"" + " -j DROP";

  if (sudo)
    command = "sudo " + command;

  auto result = utility::execCommand(command);
  if (result.exitCode != 0) {
    primaryLog.log("Failed to remove Rule for  UnBanning IP:" + ip);
    return false;
  }

  primaryLog.log("Succeeded in UnBanning IP:" + ip);
  return true;
}

IptablesFirewall &IptablesFirewall::getInstance(Logger &log, bool sudo) {
  static IptablesFirewall instance(log, sudo);
  return instance;
}
