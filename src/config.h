#ifndef __CONFIG__
#define __CONFIG__
#include <string>
#include <vector>
#include <unordered_map>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include "utility.h"
#include "sequence.h"

enum class firewallType {invalid=0, mock, ufw, firewalld, iptables };

class Config
{
private:
   std::string secret_key;
   std::string log_file;
   firewallType firewall;
   bool ban;
   int ban_timer;
   std::unordered_map<std::string, Sequence> sequences;
   bool valid;

   //Sets avd validates firewall type
   static firewallType setFirewallType(const std::string &firewallString);
   void populateSequencesKnockPorts(const std::string &key, const std::string &value);
   void populateSequencesUnlockPorts(const std::string &key, const std::string &port);
   //dumps all udp ports to a vector
   std::vector<int> dumpPorts() const;
   void validate();
public:
    // Can throw invalid_argument if there are errors in the config file.
    explicit Config(const std::string &filePath);
    std::string getSecretKey() const;
    std::string getLogFile() const;
    firewallType getFirewall() const;
    bool banEnabled() const;
    int getBanTimer() const;
    std::unordered_map<std::string, Sequence> getSequences() const;
    bool isValid() const;
    ~Config() = default;
};
#endif