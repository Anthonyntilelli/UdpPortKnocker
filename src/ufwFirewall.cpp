#include "ufwFirewall.h"

UfwFirewall::UfwFirewall(Logger &log, bool useSudo):mtx{},primaryLog{log},sudo{useSudo} {
  std::lock_guard{mtx};
  // version check
  utility::CommandResult result {};
  if (sudo) result = utility::execCommand("sudo ufw --version");
  else result = utility::execCommand("ufw --version");
  if(result.exitCode == 127) throw std::runtime_error{"UFW is not installed."};
  if(result.exitCode != 0)  throw std::runtime_error{"UFW Error of somekind: During version check."};
  size_t frontPos = result.output.find(' ');
  size_t endPos = result.output.find('.');
  if (frontPos != std::string::npos && endPos != std::string::npos) {
    auto majorVersion = result.output.substr(frontPos+1, endPos - frontPos - 1);
    if(majorVersion != "0") {
      log.log("Warning: This program was was tested on ufw major version 0 of ufw it may not work on newer versions.");
    }
  } else {
    log.log("Version check on ufw failed, will attempt anyway.");
  }

  //enable check
  utility::CommandResult result2 {};
  if (sudo) result2 = utility::execCommand("sudo ufw status");
  else result2 = utility::execCommand("ufw status");
  if(result2.exitCode != 0)  throw std::runtime_error{"UFW Error of somekind During enabled check."};
  auto inactivePos = result2.output.find("inactive");
  if (inactivePos != std::string::npos) throw std::runtime_error{"UFW is not enabled"};

  primaryLog.log("Server Starting: UfwFirewall class is created");

}

//Works for ipv4 only
bool UfwFirewall::validateIp(std::string ipaAddr) {
  unsigned char buf[sizeof(struct in6_addr)];
  auto check = inet_pton(AF_INET, ipaAddr.c_str(), buf);
  if (check <= 0) return false;
  return true;
}

bool UfwFirewall::validatePort(size_t port) {return (port >= 0 && port <= 65535); }

UfwFirewall::~UfwFirewall() {
  // TODO detect and remove remaining firewall rules
  try {
    primaryLog.log("Server Stopping: UfwFirewall class is destroyed");
  } catch(...){
    std::cerr << "Error in closing the Logger Class." << std::endl;
  }
}

bool UfwFirewall::allow_in(std::string ip, Protocol protocol, size_t port) {
  std::lock_guard{mtx};
  auto valid = (validateIp(ip) && validatePort(port));
  if(!valid) {
    primaryLog.log("Warning: Invalid port or ip sent to UFW firewall allow_in rule");
    return false;
  }
  auto proto = (protocol == Protocol::udp) ? "udp" : "tcp";
  auto sPort = std::to_string(port);
  auto command =   "ufw allow from " + ip + " to any port " + sPort + " proto " + proto + " comment " + "'udpPortKnocker," + ip + "," + proto + "," + sPort + "'";
  if (sudo) command = "sudo " + command;
  
  auto result = utility::execCommand(command);
  

  if(result.exitCode == 0 ) {
    primaryLog.log("Succeeded in Adding for  Allow IP:" + ip + " Protocol: " + proto + "Port: " + std::to_string(port));
    return true;
  }
  primaryLog.log("Failed to add Rule for  Allow IP:" + ip + " Protocol: " + proto + "Port: " + std::to_string(port));
  return false;
}

bool UfwFirewall::removeRule(std::string ip, Protocol protocol, size_t port) {
  // std::lock_guard{mtx};
  // auto valid = (validateIp(ip) && validatePort(port));
  // if(!valid) {
  //   primaryLog.log("Warning: Invalid port or ip sent to UFW firewall remove_rule rule");
  //   return false;
  // }
  // auto proto = (protocol == Protocol::udp) ? "udp" : "tcp";
  // auto sPort = std::to_string(port);
  // // NOTE: Comment MUST match for delete to work
  // //sudo ufw delete allow from 2.2.2.2 to any port 55 proto tcp comment 'udpPortKnocker,2.2.2.2,tcp,55'
  // auto command =   "ufw allow from " + ip + " to any port " + sPort + " proto " + proto + " comment " + "'udpPortKnocker," + ip + "," + proto + "," + sPort + "'";
  // if (sudo) command = "sudo " + command;
  
  // auto result = utility::execCommand(command);
  

  // if(result.exitCode == 0 ) {
  //   primaryLog.log("Succeeded in Adding for  Allow IP:" + ip + " Protocol: " + proto + "Port: " + std::to_string(port));
  //   return true;
  // }
  // primaryLog.log("Failed to add Rule for  Allow IP:" + ip + " Protocol: " + proto + "Port: " + std::to_string(port));
  // return false;

  return false;
}

bool UfwFirewall::block(std::string ip) { return false; }
bool UfwFirewall::unblock(std::string ip) { return false; }

UfwFirewall& UfwFirewall::getInstance(Logger &log, bool sudo){
    static UfwFirewall instance(log, sudo);
    return instance;
}
