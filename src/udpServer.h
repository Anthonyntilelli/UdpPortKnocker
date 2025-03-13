#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <arpa/inet.h>
#include <array>
#include <bits/stdc++.h>
#include <map>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

struct message {
  std::string message;
  uint16_t port;
  std::string ipaddress;
};

// IP V4 only
class UdpServer {
private:
  static constexpr size_t BUFFERSIZE = 4096;
  static constexpr size_t MAXEVENTS = 10;

  // file descriptor and port
  std::map<int, uint16_t> sockfds;
  int epoll_fd;
  bool socketsOpen;

  int makeUdpPort(uint16_t port) const;
  void closeAllSocketFd();

public:
  UdpServer(std::vector<uint16_t> ports);
  ~UdpServer();
  std::vector<message> receive();
};
#endif
