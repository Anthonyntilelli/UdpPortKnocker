#ifndef __CONFIG__
#define __CONFIG__
#include <string>
#include <vector>
#include <unordered_map>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "utility.cpp"

enum class firewallType {invalid=0, mock, ufw, firewalld, iptables };

struct sequence {
    int unlock_port;
    bool tcp; // false is udp
    std::vector<int> knockPorts;
    
    // Check if the port is in range of actuall ports and pushes back if valid
    // Throws std::invalid_argument if invalid
    void addPortToSequence(const int port){
        if (port >= 1024 && port <= 65535) knockPorts.push_back(port);
        else throw std::invalid_argument("Sequence port is not a valid, it must be in between 1024 and 65535");
    }
};

class Config
{
private:
   std::string secret_key;
   std::string log_file;
   firewallType firewall;
   bool ban;
   int ban_timer;
   std::unordered_map<std::string, sequence> sequences;

   //Sets avd validates firewall type
   firewallType setFirewallType(const std::string &firewall);
   void populateSequencesKnockPorts(const std::string &key, const std::string &value);
public:
    // Can thow invalid_argument if there are errors in the config file.
    Config(const std::string &filePath);
    std::string getSecretKey() const;
    std::string getLogFile() const;
    firewallType getFirewall() const;
    bool banEnabled() const;
    int getBanTimer() const;
    std::unordered_map<std::string, sequence> getSequences() const;
    ~Config();
};
#endif