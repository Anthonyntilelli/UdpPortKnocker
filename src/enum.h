#ifndef ENUM_H
#define ENUM_H

enum class firewallType { invalid = 0, mock, ufw, firewalld, iptables };
enum class Protocol : char { tcp = 't', udp = 'u' };

#endif