#ifndef CONFIG_H
#define CONFIG_H
#include "enum.h"
#include "sequence.h"
#include "utility.h"
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Config {
private:
  std::string secret_key;
  std::string log_file;
  firewallType firewall;
  bool ban;
  int ban_timer;
  std::unordered_map<std::string, Sequence> sequences;
  bool valid;
  bool sudo;

  // Sets avd validates firewall type
  static firewallType setFirewallType(const std::string &firewallString);
  void populateSequencesKnockPorts(const std::string &key,
                                   const std::string &value);
  void populateSequencesUnlockPorts(const std::string &key,
                                    const std::string &port);
  // dumps all udp ports to a vector
  std::vector<int> dumpPorts() const;
  void validate();

public:
  // Can throw invalid_argument if there are errors in the config file.
  explicit Config();
  void load(const std::string &filePath);
  std::string getSecretKey() const;
  std::string getLogFile() const;
  firewallType getFirewall() const;
  bool banEnabled() const;
  int getBanTimer() const;
  bool getSudo() const;
  std::unordered_map<std::string, Sequence> getSequences() const;
  bool isValid() const;
  ~Config() = default;
};
#endif